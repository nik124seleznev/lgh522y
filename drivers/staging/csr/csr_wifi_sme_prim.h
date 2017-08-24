/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_SME_PRIM_H__
#define CSR_WIFI_SME_PRIM_H__

#include <linux/types.h>
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"

#define CSR_WIFI_SME_PRIM                                               (0x0404)

typedef CsrPrim CsrWifiSmePrim;


/*                                                                              

      
                              

             
                                               

       
                                      
                                                    
                                          
                                                                          
                                         
                                                                        
                                        
                              

                                                                              */
typedef u8 CsrWifiSme80211NetworkType;
#define CSR_WIFI_SME_80211_NETWORK_TYPE_DS       ((CsrWifiSme80211NetworkType) 0x00)
#define CSR_WIFI_SME_80211_NETWORK_TYPE_OFDM24   ((CsrWifiSme80211NetworkType) 0x01)
#define CSR_WIFI_SME_80211_NETWORK_TYPE_OFDM5    ((CsrWifiSme80211NetworkType) 0x02)
#define CSR_WIFI_SME_80211_NETWORK_TYPE_AUTO     ((CsrWifiSme80211NetworkType) 0x03)

/*                                                                              

      
                              

             
                                              

       
                                            
                                                                               
                              
                                           
                                             

                                                                              */
typedef u8 CsrWifiSme80211PrivacyMode;
#define CSR_WIFI_SME_80211_PRIVACY_MODE_DISABLED   ((CsrWifiSme80211PrivacyMode) 0x00)
#define CSR_WIFI_SME_80211_PRIVACY_MODE_ENABLED    ((CsrWifiSme80211PrivacyMode) 0x01)

/*                                                                              

      
                              

             
                                                              

       
                                          
                                                                               
                                           
                                           
                                                                     
                                
                                       
                                                                               
                             
                                        
                                                                           
                             
                                       
                                                                            
                                                     
                                            
                                                                            
                                                                             
                            

                                                                              */
typedef u8 CsrWifiSme80211dTrustLevel;
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_STRICT     ((CsrWifiSme80211dTrustLevel) 0x01)
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_ADJUNCT    ((CsrWifiSme80211dTrustLevel) 0x02)
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_BSS        ((CsrWifiSme80211dTrustLevel) 0x03)
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_IBSS       ((CsrWifiSme80211dTrustLevel) 0x04)
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_MIB        ((CsrWifiSme80211dTrustLevel) 0x05)
#define CSR_WIFI_SME_80211D_TRUST_LEVEL_DISABLED   ((CsrWifiSme80211dTrustLevel) 0x06)

/*                                                                              

      
                       

             
                      

       
                                           
                                            

                                                                              */
typedef u8 CsrWifiSmeAmpStatus;
#define CSR_WIFI_SME_AMP_ACTIVE     ((CsrWifiSmeAmpStatus) 0x00)
#define CSR_WIFI_SME_AMP_INACTIVE   ((CsrWifiSmeAmpStatus) 0x01)

/*                                                                              

      
                      

             
                                      

       
                                     
                                                                                
                                                                
                                       
                                                      
                                    
                                                                  
                                       
                                                                           
                                     
                                                                   
                                        
                                                                            
                                     
                                                        
                                   
                                                                   
                                      
                                                                            
                                        
                                    

                                                                              */
typedef u16 CsrWifiSmeAuthMode;
#define CSR_WIFI_SME_AUTH_MODE_80211_OPEN      ((CsrWifiSmeAuthMode) 0x0001)
#define CSR_WIFI_SME_AUTH_MODE_80211_SHARED    ((CsrWifiSmeAuthMode) 0x0002)
#define CSR_WIFI_SME_AUTH_MODE_8021X_WPA       ((CsrWifiSmeAuthMode) 0x0004)
#define CSR_WIFI_SME_AUTH_MODE_8021X_WPAPSK    ((CsrWifiSmeAuthMode) 0x0008)
#define CSR_WIFI_SME_AUTH_MODE_8021X_WPA2      ((CsrWifiSmeAuthMode) 0x0010)
#define CSR_WIFI_SME_AUTH_MODE_8021X_WPA2PSK   ((CsrWifiSmeAuthMode) 0x0020)
#define CSR_WIFI_SME_AUTH_MODE_8021X_CCKM      ((CsrWifiSmeAuthMode) 0x0040)
#define CSR_WIFI_SME_AUTH_MODE_WAPI_WAI        ((CsrWifiSmeAuthMode) 0x0080)
#define CSR_WIFI_SME_AUTH_MODE_WAPI_WAIPSK     ((CsrWifiSmeAuthMode) 0x0100)
#define CSR_WIFI_SME_AUTH_MODE_8021X_OTHER1X   ((CsrWifiSmeAuthMode) 0x0200)

/*                                                                              

      
                            

             
                                              

       
                                         
                                                           
                                     
                                                                              
                                             
                                            
                                              
                                  

                                                                              */
typedef u8 CsrWifiSmeBasicUsability;
#define CSR_WIFI_SME_BASIC_USABILITY_UNUSABLE        ((CsrWifiSmeBasicUsability) 0x00)
#define CSR_WIFI_SME_BASIC_USABILITY_POOR            ((CsrWifiSmeBasicUsability) 0x01)
#define CSR_WIFI_SME_BASIC_USABILITY_SATISFACTORY    ((CsrWifiSmeBasicUsability) 0x02)
#define CSR_WIFI_SME_BASIC_USABILITY_NOT_CONNECTED   ((CsrWifiSmeBasicUsability) 0x03)

/*                                                                              

      
                     

             
                          

       
                                        
                                        
                               
                                               
                                 
                                              
                             
                                  

                                                                              */
typedef u8 CsrWifiSmeBssType;
#define CSR_WIFI_SME_BSS_TYPE_INFRASTRUCTURE   ((CsrWifiSmeBssType) 0x00)
#define CSR_WIFI_SME_BSS_TYPE_ADHOC            ((CsrWifiSmeBssType) 0x01)
#define CSR_WIFI_SME_BSS_TYPE_ANY_BSS          ((CsrWifiSmeBssType) 0x02)
#define CSR_WIFI_SME_BSS_TYPE_P2P              ((CsrWifiSmeBssType) 0x03)

/*                                                                              

      
                        

             
                                          
                     

       
                                     
                                                           
                                
                                                     
                                        
                                                    
                                
                                                                     

                                                                              */
typedef u8 CsrWifiSmeCoexScheme;
#define CSR_WIFI_SME_COEX_SCHEME_DISABLED      ((CsrWifiSmeCoexScheme) 0x00)
#define CSR_WIFI_SME_COEX_SCHEME_CSR           ((CsrWifiSmeCoexScheme) 0x01)
#define CSR_WIFI_SME_COEX_SCHEME_CSR_CHANNEL   ((CsrWifiSmeCoexScheme) 0x02)
#define CSR_WIFI_SME_COEX_SCHEME_PTA           ((CsrWifiSmeCoexScheme) 0x03)

/*                                                                              

      
                               

             
                                                             
                                                                             
                                                                           
                    

       
                                         
                                                                               
                                         
                                                                             
                                                              
                                        
                                                                               
                                                                             
                              
                                                  
                                                                            
                                                                   

                                                                              */
typedef u8 CsrWifiSmeControlIndication;
#define CSR_WIFI_SME_CONTROL_INDICATION_ERROR            ((CsrWifiSmeControlIndication) 0x01)
#define CSR_WIFI_SME_CONTROL_INDICATION_EXIT             ((CsrWifiSmeControlIndication) 0x02)
#define CSR_WIFI_SME_CONTROL_INDICATION_USER_REQUESTED   ((CsrWifiSmeControlIndication) 0x03)

/*                                                                              

      
                               

             
                            

       
                                         
                                                                                
                                                                               
                                                                        
                                             
                                                    
                                              
                                                      
                                                 
                                                                             
                                                                                
                                                                                
                                                                          

                                                                              */
typedef u8 CsrWifiSmeCtsProtectionType;
#define CSR_WIFI_SME_CTS_PROTECTION_AUTOMATIC           ((CsrWifiSmeCtsProtectionType) 0x00)
#define CSR_WIFI_SME_CTS_PROTECTION_FORCE_ENABLED       ((CsrWifiSmeCtsProtectionType) 0x01)
#define CSR_WIFI_SME_CTS_PROTECTION_FORCE_DISABLED      ((CsrWifiSmeCtsProtectionType) 0x02)
#define CSR_WIFI_SME_CTS_PROTECTION_AUTOMATIC_NO_OLBC   ((CsrWifiSmeCtsProtectionType) 0x03)

/*                                                                              

      
                            

             
                                                       

       
                                      
                                             
                                       
                                                    
                                        
                                                                 
                                            

                                                                              */
typedef u8 CsrWifiSmeD3AutoScanMode;
#define CSR_WIFI_SME_D3AUTO_SCAN_MODE_PSON     ((CsrWifiSmeD3AutoScanMode) 0x00)
#define CSR_WIFI_SME_D3AUTO_SCAN_MODE_PSOFF    ((CsrWifiSmeD3AutoScanMode) 0x01)
#define CSR_WIFI_SME_D3AUTO_SCAN_MODE_PSAUTO   ((CsrWifiSmeD3AutoScanMode) 0x02)

/*                                                                              

      
                        

             
                                         
                                                                  
                                                   

       
                                       
                                      
                                                 
                                                                      
                                                  
                                                                       
                                                
                                                           
                                                
                                                           
                                                
                                                           
                                              
                                                                   
                                               
                                                                    
                                             
                                                          
                                             
                                                        
                                             
                                                        

                                                                              */
typedef u16 CsrWifiSmeEncryption;
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_NONE              ((CsrWifiSmeEncryption) 0x0000)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_PAIRWISE_WEP40    ((CsrWifiSmeEncryption) 0x0001)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_PAIRWISE_WEP104   ((CsrWifiSmeEncryption) 0x0002)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_PAIRWISE_TKIP     ((CsrWifiSmeEncryption) 0x0004)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_PAIRWISE_CCMP     ((CsrWifiSmeEncryption) 0x0008)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_PAIRWISE_SMS4     ((CsrWifiSmeEncryption) 0x0010)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_GROUP_WEP40       ((CsrWifiSmeEncryption) 0x0020)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_GROUP_WEP104      ((CsrWifiSmeEncryption) 0x0040)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_GROUP_TKIP        ((CsrWifiSmeEncryption) 0x0080)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_GROUP_CCMP        ((CsrWifiSmeEncryption) 0x0100)
#define CSR_WIFI_SME_ENCRYPTION_CIPHER_GROUP_SMS4        ((CsrWifiSmeEncryption) 0x0200)

/*                                                                              

      
                                     

             
                                                   

       
                                                              
                                                                                
                                                           
                                                                  

                                                                              */
typedef u8 CsrWifiSmeFirmwareDriverInterface;
#define CSR_WIFI_SME_FIRMWARE_DRIVER_INTERFACE_UNIT_DATA_INTERFACE   ((CsrWifiSmeFirmwareDriverInterface) 0x00)
#define CSR_WIFI_SME_FIRMWARE_DRIVER_INTERFACE_PACKET_INTERFACE      ((CsrWifiSmeFirmwareDriverInterface) 0x01)

/*                                                                              

      
                           

             
                          

       
                                       
                                                              
                                           
                                                                        
                                                
                                    

                                                                              */
typedef u8 CsrWifiSmeHostPowerMode;
#define CSR_WIFI_SME_HOST_POWER_MODE_ACTIVE            ((CsrWifiSmeHostPowerMode) 0x00)
#define CSR_WIFI_SME_HOST_POWER_MODE_POWER_SAVE        ((CsrWifiSmeHostPowerMode) 0x01)
#define CSR_WIFI_SME_HOST_POWER_MODE_FULL_POWER_SAVE   ((CsrWifiSmeHostPowerMode) 0x02)

/*                                                                              

      
                             

             
                                             

       
                                         
                                             
                                                    
                                             
                                                          
                                             
                                                           
                                             
                                                          
                                             
                                             
                                             
                                                    
                                             
                                                    
                                             
                                                         
                                             
                                                               
                                             
                                                                
                                             
                                                                  
                                             
                                                             
                                             
                                                     
                                             
                                                           
                                             
                                                          
                                             
                                                             
                                             
                                                      
                                             
                                                         
                                             
                                              
                                             
                                                           
                                             
                                                            
                                             
                                                   
                                             
                                                           
                                             
                                                             
                                             
                                                        
                                             
                                                            
                                             
                                                       
                                             
                                                        
                                             
                                              
                                             
                                        
                                             
                                         
                                             
                                        
                                             
                                            
                                             
                                            
                                             
                                             
                                             
                                         
                                             
                                                 
                                             
                                                                 
                                             
                                                                    
                                             
                                                                         
                                             
                                                          
                                             
                                                        
                                             
                                                             
                                             
                                                            
                                             
                                                                       
                                             

                                                                              */
typedef u16 CsrWifiSmeIEEE80211Reason;
#define CSR_WIFI_SME_IEEE80211_REASON_SUCCESS                                   ((CsrWifiSmeIEEE80211Reason) 0x0000)
#define CSR_WIFI_SME_IEEE80211_REASON_UNSPECIFIED_REASON                        ((CsrWifiSmeIEEE80211Reason) 0x0001)
#define CSR_WIFI_SME_IEEE80211_REASON_AUTHENTICATION_NOT_VALID                  ((CsrWifiSmeIEEE80211Reason) 0x0002)
#define CSR_WIFI_SME_IEEE80211_REASON_DEAUTHENTICATED_LEAVE_BSS                 ((CsrWifiSmeIEEE80211Reason) 0x0003)
#define CSR_WIFI_SME_IEEE80211_REASON_DISASSOCIATED_INACTIVITY                  ((CsrWifiSmeIEEE80211Reason) 0x0004)
#define CSR_WIFI_SME_IEEE80211_REASON_AP_OVERLOAD                               ((CsrWifiSmeIEEE80211Reason) 0x0005)
#define CSR_WIFI_SME_IEEE80211_REASON_CLASS_2FRAME_ERROR                        ((CsrWifiSmeIEEE80211Reason) 0x0006)
#define CSR_WIFI_SME_IEEE80211_REASON_CLASS_3FRAME_ERROR                        ((CsrWifiSmeIEEE80211Reason) 0x0007)
#define CSR_WIFI_SME_IEEE80211_REASON_DISASSOCIATED_LEAVE_BSS                   ((CsrWifiSmeIEEE80211Reason) 0x0008)
#define CSR_WIFI_SME_IEEE80211_REASON_ASSOCIATION_NOT_AUTHENTICATED             ((CsrWifiSmeIEEE80211Reason) 0x0009)
#define CSR_WIFI_SME_IEEE80211_REASON_DISASSOCIATED_POWER_CAPABILITY            ((CsrWifiSmeIEEE80211Reason) 0x000a)
#define CSR_WIFI_SME_IEEE80211_REASON_DISASSOCIATED_SUPPORTED_CHANNELS          ((CsrWifiSmeIEEE80211Reason) 0x000b)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_INFORMATION_ELEMENT               ((CsrWifiSmeIEEE80211Reason) 0x000d)
#define CSR_WIFI_SME_IEEE80211_REASON_MICHAEL_MIC_FAILURE                       ((CsrWifiSmeIEEE80211Reason) 0x000e)
#define CSR_WIFI_SME_IEEE80211_REASON_FOURWAY_HANDSHAKE_TIMEOUT                 ((CsrWifiSmeIEEE80211Reason) 0x000f)
#define CSR_WIFI_SME_IEEE80211_REASON_GROUP_KEY_UPDATE_TIMEOUT                  ((CsrWifiSmeIEEE80211Reason) 0x0010)
#define CSR_WIFI_SME_IEEE80211_REASON_HANDSHAKE_ELEMENT_DIFFERENT               ((CsrWifiSmeIEEE80211Reason) 0x0011)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_GROUP_CIPHER                      ((CsrWifiSmeIEEE80211Reason) 0x0012)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_PAIRWISE_CIPHER                   ((CsrWifiSmeIEEE80211Reason) 0x0013)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_AKMP                              ((CsrWifiSmeIEEE80211Reason) 0x0014)
#define CSR_WIFI_SME_IEEE80211_REASON_UNSUPPORTED_RSN_IEVERSION                 ((CsrWifiSmeIEEE80211Reason) 0x0015)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_RSN_IECAPABILITIES                ((CsrWifiSmeIEEE80211Reason) 0x0016)
#define CSR_WIFI_SME_IEEE80211_REASON_DOT1X_AUTH_FAILED                         ((CsrWifiSmeIEEE80211Reason) 0x0017)
#define CSR_WIFI_SME_IEEE80211_REASON_CIPHER_REJECTED_BY_POLICY                 ((CsrWifiSmeIEEE80211Reason) 0x0018)
#define CSR_WIFI_SME_IEEE80211_REASON_SERVICE_CHANGE_PRECLUDES_TS               ((CsrWifiSmeIEEE80211Reason) 0x001F)
#define CSR_WIFI_SME_IEEE80211_REASON_QOS_UNSPECIFIED_REASON                    ((CsrWifiSmeIEEE80211Reason) 0x0020)
#define CSR_WIFI_SME_IEEE80211_REASON_QOS_INSUFFICIENT_BANDWIDTH                ((CsrWifiSmeIEEE80211Reason) 0x0021)
#define CSR_WIFI_SME_IEEE80211_REASON_QOS_EXCESSIVE_NOT_ACK                     ((CsrWifiSmeIEEE80211Reason) 0x0022)
#define CSR_WIFI_SME_IEEE80211_REASON_QOS_TXOPLIMIT_EXCEEDED                    ((CsrWifiSmeIEEE80211Reason) 0x0023)
#define CSR_WIFI_SME_IEEE80211_REASON_QSTA_LEAVING                              ((CsrWifiSmeIEEE80211Reason) 0x0024)
#define CSR_WIFI_SME_IEEE80211_REASON_END_TS                                    ((CsrWifiSmeIEEE80211Reason) 0x0025)
#define CSR_WIFI_SME_IEEE80211_REASON_END_DLS                                   ((CsrWifiSmeIEEE80211Reason) 0x0025)
#define CSR_WIFI_SME_IEEE80211_REASON_END_BA                                    ((CsrWifiSmeIEEE80211Reason) 0x0025)
#define CSR_WIFI_SME_IEEE80211_REASON_UNKNOWN_TS                                ((CsrWifiSmeIEEE80211Reason) 0x0026)
#define CSR_WIFI_SME_IEEE80211_REASON_UNKNOWN_BA                                ((CsrWifiSmeIEEE80211Reason) 0x0026)
#define CSR_WIFI_SME_IEEE80211_REASON_UNKNOWN_DLS                               ((CsrWifiSmeIEEE80211Reason) 0x0026)
#define CSR_WIFI_SME_IEEE80211_REASON_TIMEOUT                                   ((CsrWifiSmeIEEE80211Reason) 0x0027)
#define CSR_WIFI_SME_IEEE80211_REASON_STAKEY_MISMATCH                           ((CsrWifiSmeIEEE80211Reason) 0x002d)
#define CSR_WIFI_SME_IEEE80211_REASON_UNICAST_KEY_NEGOTIATION_TIMEOUT           ((CsrWifiSmeIEEE80211Reason) 0xf019)
#define CSR_WIFI_SME_IEEE80211_REASON_MULTICAST_KEY_ANNOUNCEMENT_TIMEOUT        ((CsrWifiSmeIEEE80211Reason) 0xf01a)
#define CSR_WIFI_SME_IEEE80211_REASON_INCOMPATIBLE_UNICAST_KEY_NEGOTIATION_IE   ((CsrWifiSmeIEEE80211Reason) 0xf01b)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_MULTICAST_CIPHER                  ((CsrWifiSmeIEEE80211Reason) 0xf01c)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_UNICAST_CIPHER                    ((CsrWifiSmeIEEE80211Reason) 0xf01d)
#define CSR_WIFI_SME_IEEE80211_REASON_UNSUPPORTED_WAPI_IE_VERSION               ((CsrWifiSmeIEEE80211Reason) 0xf01e)
#define CSR_WIFI_SME_IEEE80211_REASON_INVALID_WAPI_CAPABILITY_IE                ((CsrWifiSmeIEEE80211Reason) 0xf01f)
#define CSR_WIFI_SME_IEEE80211_REASON_WAI_CERTIFICATE_AUTHENTICATION_FAILED     ((CsrWifiSmeIEEE80211Reason) 0xf020)

/*                                                                              

      
                             

             
                                             

       
                                         
                                             
                                                     
                                             
                                                               
                                             
                                                                     
                                             
                                                         
                                             
                                                                 
                                             
                                                                                
                                             
                                                           
                                             
                                                                
                                             
                                                          
                                             
                                                              
                                             
                                                                 
                                             
                                                                  
                                             
                                                                  
                                             
                                                                      
                                             
                                                                       
                                             
                                                                         
                                             
                                                             
                                             
                                                           
                                             
                                                       
                                             
                                                  
                                             
                                                                
                                             
                                                                          
                                             
                                                                          
                                             
                                         
                                             
                                                                   
                                             
                                                                
                                             
                                                      
                                             
                                                            
                                             
                                         
                                             
                                                    
                                             
                                                                       
                                             
                                                     
                                             
                                                               
                                             
                                                                  
                                             
                                                       
                                             
                                                                  
                                             
                                                                 
                                             
                                                          
                                             
                                                           
                                             
                                             
                                             
                                             
                                             
                                          
                                             
                                                                    
                                             
                                                               
                                             
                                               
                                             
                                              
                                             
                                              
                                             
                                                         
                                             
                                              
                                             
                                                        
                                             
                                                          
                                             
                                         
                                             
                                                                
                                             
                                                               
                                             
                                               
                                             
                                                      
                                             
                                                           
                                             
                                                             
                                             
                                                     
                                             
                                                        
                                             
                                                          
                                             
                                                             
                                             
                                                            
                                             

                                                                              */
typedef u16 CsrWifiSmeIEEE80211Result;
#define CSR_WIFI_SME_IEEE80211_RESULT_SUCCESS                                          ((CsrWifiSmeIEEE80211Result) 0x0000)
#define CSR_WIFI_SME_IEEE80211_RESULT_UNSPECIFIED_FAILURE                              ((CsrWifiSmeIEEE80211Result) 0x0001)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_CAPABILITIES_MISMATCH                    ((CsrWifiSmeIEEE80211Result) 0x000a)
#define CSR_WIFI_SME_IEEE80211_RESULT_REASSOCIATION_DENIED_NO_ASSOCIATION              ((CsrWifiSmeIEEE80211Result) 0x000b)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_EXTERNAL_REASON                          ((CsrWifiSmeIEEE80211Result) 0x000c)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_AUTHENTICATION_MISMATCH                  ((CsrWifiSmeIEEE80211Result) 0x000d)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_INVALID_AUTHENTICATION_SEQUENCE_NUMBER   ((CsrWifiSmeIEEE80211Result) 0x000e)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_CHALLENGE_FAILURE                        ((CsrWifiSmeIEEE80211Result) 0x000f)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_AUTHENTICATION_TIMEOUT                   ((CsrWifiSmeIEEE80211Result) 0x0010)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_AP_OUT_OF_MEMORY                         ((CsrWifiSmeIEEE80211Result) 0x0011)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_BASIC_RATES_MISMATCH                     ((CsrWifiSmeIEEE80211Result) 0x0012)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_SHORT_PREAMBLE_REQUIRED                  ((CsrWifiSmeIEEE80211Result) 0x0013)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_PBCC_MODULATION_REQUIRED                 ((CsrWifiSmeIEEE80211Result) 0x0014)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_CHANNEL_AGILITY_REQUIRED                 ((CsrWifiSmeIEEE80211Result) 0x0015)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_SPECTRUM_MANAGEMENT_REQUIRED             ((CsrWifiSmeIEEE80211Result) 0x0016)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_POWER_CAPABILITY_UNACCEPTABLE            ((CsrWifiSmeIEEE80211Result) 0x0017)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_SUPPORTED_CHANNELS_UNACCEPTABLE          ((CsrWifiSmeIEEE80211Result) 0x0018)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_SHORT_SLOT_REQUIRED                      ((CsrWifiSmeIEEE80211Result) 0x0019)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_DSSS_OFDMREQUIRED                        ((CsrWifiSmeIEEE80211Result) 0x001a)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_NO_HT_SUPPORT                            ((CsrWifiSmeIEEE80211Result) 0x001b)
#define CSR_WIFI_SME_IEEE80211_RESULT_R0KH_UNREACHABLE                                 ((CsrWifiSmeIEEE80211Result) 0x001c)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_PCO_TRANSITION_SUPPORT                   ((CsrWifiSmeIEEE80211Result) 0x001d)
#define CSR_WIFI_SME_IEEE80211_RESULT_ASSOCIATION_REQUEST_REJECTED_TEMPORARILY         ((CsrWifiSmeIEEE80211Result) 0x001e)
#define CSR_WIFI_SME_IEEE80211_RESULT_ROBUST_MANAGEMENT_FRAME_POLICY_VIOLATION         ((CsrWifiSmeIEEE80211Result) 0x001f)
#define CSR_WIFI_SME_IEEE80211_RESULT_FAILURE                                          ((CsrWifiSmeIEEE80211Result) 0x0020)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_AP_BANDWIDTH_INSUFFICIENT                ((CsrWifiSmeIEEE80211Result) 0x0021)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_POOR_OPERATING_CHANNEL                   ((CsrWifiSmeIEEE80211Result) 0x0022)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_QOS_REQUIRED                             ((CsrWifiSmeIEEE80211Result) 0x0023)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_REASON_UNSPECIFIED                       ((CsrWifiSmeIEEE80211Result) 0x0025)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED                                          ((CsrWifiSmeIEEE80211Result) 0x0025)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_PARAMETERS                               ((CsrWifiSmeIEEE80211Result) 0x0026)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_WITH_SUGGESTED_TSPEC_CHANGES            ((CsrWifiSmeIEEE80211Result) 0x0027)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_INVALID_IE                              ((CsrWifiSmeIEEE80211Result) 0x0028)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_INVALID_GROUP_CIPHER                    ((CsrWifiSmeIEEE80211Result) 0x0029)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_INVALID_PAIRWISE_CIPHER                 ((CsrWifiSmeIEEE80211Result) 0x002a)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_INVALID_AKMP                            ((CsrWifiSmeIEEE80211Result) 0x002b)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_UNSUPPORTED_RSN_VERSION                 ((CsrWifiSmeIEEE80211Result) 0x002c)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_INVALID_RSN_CAPABILITY                  ((CsrWifiSmeIEEE80211Result) 0x002d)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_SECURITY_POLICY                         ((CsrWifiSmeIEEE80211Result) 0x002e)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_FOR_DELAY_PERIOD                        ((CsrWifiSmeIEEE80211Result) 0x002f)
#define CSR_WIFI_SME_IEEE80211_RESULT_NOT_ALLOWED                                      ((CsrWifiSmeIEEE80211Result) 0x0030)
#define CSR_WIFI_SME_IEEE80211_RESULT_NOT_PRESENT                                      ((CsrWifiSmeIEEE80211Result) 0x0031)
#define CSR_WIFI_SME_IEEE80211_RESULT_NOT_QSTA                                         ((CsrWifiSmeIEEE80211Result) 0x0032)
#define CSR_WIFI_SME_IEEE80211_RESULT_REJECTED_LISTEN_INTERVAL_TOO_LARGE               ((CsrWifiSmeIEEE80211Result) 0x0033)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_FT_ACTION_FRAME_COUNT                    ((CsrWifiSmeIEEE80211Result) 0x0034)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_PMKID                                    ((CsrWifiSmeIEEE80211Result) 0x0035)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_MDIE                                     ((CsrWifiSmeIEEE80211Result) 0x0036)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_FTIE                                     ((CsrWifiSmeIEEE80211Result) 0x0037)
#define CSR_WIFI_SME_IEEE80211_RESULT_UNSPECIFIED_QOS_FAILURE                          ((CsrWifiSmeIEEE80211Result) 0x00c8)
#define CSR_WIFI_SME_IEEE80211_RESULT_WRONG_POLICY                                     ((CsrWifiSmeIEEE80211Result) 0x00c9)
#define CSR_WIFI_SME_IEEE80211_RESULT_INSUFFICIENT_BANDWIDTH                           ((CsrWifiSmeIEEE80211Result) 0x00ca)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_TSPEC_PARAMETERS                         ((CsrWifiSmeIEEE80211Result) 0x00cb)
#define CSR_WIFI_SME_IEEE80211_RESULT_TIMEOUT                                          ((CsrWifiSmeIEEE80211Result) 0x8000)
#define CSR_WIFI_SME_IEEE80211_RESULT_TOO_MANY_SIMULTANEOUS_REQUESTS                   ((CsrWifiSmeIEEE80211Result) 0x8001)
#define CSR_WIFI_SME_IEEE80211_RESULT_BSS_ALREADY_STARTED_OR_JOINED                    ((CsrWifiSmeIEEE80211Result) 0x8002)
#define CSR_WIFI_SME_IEEE80211_RESULT_NOT_SUPPORTED                                    ((CsrWifiSmeIEEE80211Result) 0x8003)
#define CSR_WIFI_SME_IEEE80211_RESULT_TRANSMISSION_FAILURE                             ((CsrWifiSmeIEEE80211Result) 0x8004)
#define CSR_WIFI_SME_IEEE80211_RESULT_REFUSED_NOT_AUTHENTICATED                        ((CsrWifiSmeIEEE80211Result) 0x8005)
#define CSR_WIFI_SME_IEEE80211_RESULT_RESET_REQUIRED_BEFORE_START                      ((CsrWifiSmeIEEE80211Result) 0x8006)
#define CSR_WIFI_SME_IEEE80211_RESULT_LM_INFO_UNAVAILABLE                              ((CsrWifiSmeIEEE80211Result) 0x8007)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_UNICAST_CIPHER                           ((CsrWifiSmeIEEE80211Result) 0xf02f)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_MULTICAST_CIPHER                         ((CsrWifiSmeIEEE80211Result) 0xf030)
#define CSR_WIFI_SME_IEEE80211_RESULT_UNSUPPORTED_WAPI_IE_VERSION                      ((CsrWifiSmeIEEE80211Result) 0xf031)
#define CSR_WIFI_SME_IEEE80211_RESULT_INVALID_WAPI_CAPABILITY_IE                       ((CsrWifiSmeIEEE80211Result) 0xf032)

/*                                                                              

      
                         

             
                                              

       
                                 
                                                                               
                                    
                                                                          
                                       
                                                                             
                                              
                                                                             
                           
                                        
                                                                              
                                       
                                                                            
                                               
                                                                              
                           
                                  
                                                                       
                                      
                                                                            
                                         
                                                                               
                                             
                                                                            
                           
                                                
                                                                               
                           
                                        
                                                                              
                                   
                                                                         
                                  
                                                                       
                                 
                                                                      
                                     
                                                                           
                                
                                                                   

                                                                              */
typedef u32 CsrWifiSmeIndications;
#define CSR_WIFI_SME_INDICATIONS_NONE                  ((CsrWifiSmeIndications) 0x00000000)
#define CSR_WIFI_SME_INDICATIONS_WIFIOFF               ((CsrWifiSmeIndications) 0x00000001)
#define CSR_WIFI_SME_INDICATIONS_SCANRESULT            ((CsrWifiSmeIndications) 0x00000002)
#define CSR_WIFI_SME_INDICATIONS_CONNECTIONQUALITY     ((CsrWifiSmeIndications) 0x00000004)
#define CSR_WIFI_SME_INDICATIONS_MEDIASTATUS           ((CsrWifiSmeIndications) 0x00000008)
#define CSR_WIFI_SME_INDICATIONS_MICFAILURE            ((CsrWifiSmeIndications) 0x00000010)
#define CSR_WIFI_SME_INDICATIONS_PMKIDCANDIDATELIST    ((CsrWifiSmeIndications) 0x00000020)
#define CSR_WIFI_SME_INDICATIONS_TSPEC                 ((CsrWifiSmeIndications) 0x00000040)
#define CSR_WIFI_SME_INDICATIONS_ROAMSTART             ((CsrWifiSmeIndications) 0x00000080)
#define CSR_WIFI_SME_INDICATIONS_ROAMCOMPLETE          ((CsrWifiSmeIndications) 0x00000100)
#define CSR_WIFI_SME_INDICATIONS_ASSOCIATIONSTART      ((CsrWifiSmeIndications) 0x00000200)
#define CSR_WIFI_SME_INDICATIONS_ASSOCIATIONCOMPLETE   ((CsrWifiSmeIndications) 0x00000400)
#define CSR_WIFI_SME_INDICATIONS_IBSSSTATION           ((CsrWifiSmeIndications) 0x00000800)
#define CSR_WIFI_SME_INDICATIONS_WIFION                ((CsrWifiSmeIndications) 0x00001000)
#define CSR_WIFI_SME_INDICATIONS_ERROR                 ((CsrWifiSmeIndications) 0x00002000)
#define CSR_WIFI_SME_INDICATIONS_INFO                  ((CsrWifiSmeIndications) 0x00004000)
#define CSR_WIFI_SME_INDICATIONS_COREDUMP              ((CsrWifiSmeIndications) 0x00008000)
#define CSR_WIFI_SME_INDICATIONS_ALL                   ((CsrWifiSmeIndications) 0xFFFFFFFF)

/*                                                                              

      
                     

             
                                 

       
                                                                    
                                                                  
                                                                         
                                                                               
                                                                 
                                                                             

                                                                              */
typedef u8 CsrWifiSmeKeyType;
#define CSR_WIFI_SME_KEY_TYPE_GROUP      ((CsrWifiSmeKeyType) 0x00)
#define CSR_WIFI_SME_KEY_TYPE_PAIRWISE   ((CsrWifiSmeKeyType) 0x01)
#define CSR_WIFI_SME_KEY_TYPE_STAKEY     ((CsrWifiSmeKeyType) 0x02)
#define CSR_WIFI_SME_KEY_TYPE_IGTK       ((CsrWifiSmeKeyType) 0x03)
#define CSR_WIFI_SME_KEY_TYPE_CCKM       ((CsrWifiSmeKeyType) 0x04)

/*                                                                              

      
                        

             
                                                                    
                                                                             
                                                                           
                    

       
                                                                        
                                                           
                                                              
                                                      

                                                                              */
typedef u8 CsrWifiSmeListAction;
#define CSR_WIFI_SME_LIST_ACTION_GET      ((CsrWifiSmeListAction) 0x00)
#define CSR_WIFI_SME_LIST_ACTION_ADD      ((CsrWifiSmeListAction) 0x01)
#define CSR_WIFI_SME_LIST_ACTION_REMOVE   ((CsrWifiSmeListAction) 0x02)
#define CSR_WIFI_SME_LIST_ACTION_FLUSH    ((CsrWifiSmeListAction) 0x03)

/*                                                                              

      
                         

             
                                   
                                                                             
                                                                           
                    

       
                                       
                                                                                
                                
                                                                                   
                                                                              
                                                                              
                                                                                   
                                                                                
                                                                                  
                                                                                
                                              
                                                                    
                                                                                
                                                                           
                                          
                                                                               
                                    
                                                                                 
                                                               
                                                                             
                                                                             
                                                         

                                                                              */
typedef u8 CsrWifiSmeMediaStatus;
#define CSR_WIFI_SME_MEDIA_STATUS_CONNECTED      ((CsrWifiSmeMediaStatus) 0x00)
#define CSR_WIFI_SME_MEDIA_STATUS_DISCONNECTED   ((CsrWifiSmeMediaStatus) 0x01)

/*                                                                              

      
                           

             
                                   

       
                                                 
                                                                              
                                                  
                                                      
                                                                            
                                                                
                                                    
                                                                        
                                                                        
                                              
                                                                             
                                                                   
                                                                      
                                                   
                                         
                                                                           
                                                                            
                               

                                                                              */
typedef u8 CsrWifiSmeP2pCapability;
#define CSR_WIFI_SME_P2P_SERVICE_DISCOVERY_CAPABILITY        ((CsrWifiSmeP2pCapability) 0x01)
#define CSR_WIFI_SME_P2P_CLIENT_DISCOVERABILITY_CAPABILITY   ((CsrWifiSmeP2pCapability) 0x02)
#define CSR_WIFI_SME_P2P_CONCURRENT_OPERATION_CAPABILITY     ((CsrWifiSmeP2pCapability) 0x04)
#define CSR_WIFI_SME_P2P_MANAGED_DEVICE_CAPABILITY           ((CsrWifiSmeP2pCapability) 0x08)
#define CSR_WIFI_SME_P2P_INVITAION_CAPABILITY                ((CsrWifiSmeP2pCapability) 0x20)

/*                                                                              

      
                                

             
                                        

       
                           
                                                                           
                                
                                   
                                     
                                      
                                                        
                                   
                                                
                                             
                                                    

                                                                              */
typedef u8 CsrWifiSmeP2pGroupCapability;
#define CSR_WIFI_P2P_GRP_CAP_GO                     ((CsrWifiSmeP2pGroupCapability) 0x01)
#define CSR_WIFI_P2P_GRP_CAP_PERSISTENT             ((CsrWifiSmeP2pGroupCapability) 0x02)
#define CSR_WIFI_P2P_GRP_CAP_INTRABSS_DIST          ((CsrWifiSmeP2pGroupCapability) 0x08)
#define CSR_WIFI_P2P_GRP_CAP_CROSS_CONN             ((CsrWifiSmeP2pGroupCapability) 0x10)
#define CSR_WIFI_P2P_GRP_CAP_PERSISTENT_RECONNECT   ((CsrWifiSmeP2pGroupCapability) 0x20)

/*                                                                              

      
                                

             
                                  

       
                                                  
                                                                     
                                                                

                                                                              */
typedef u8 CsrWifiSmeP2pNoaConfigMethod;
#define CSR_WIFI_P2P_NOA_NONE           ((CsrWifiSmeP2pNoaConfigMethod) 0x00)
#define CSR_WIFI_P2P_NOA_AUTONOMOUS     ((CsrWifiSmeP2pNoaConfigMethod) 0x01)
#define CSR_WIFI_P2P_NOA_USER_DEFINED   ((CsrWifiSmeP2pNoaConfigMethod) 0x02)

/*                                                                              

      
                     

             
                                        

       
                                                             
                                                              
                                                                            
                                                         
                                                                           
                                                         

                                                                              */
typedef u8 CsrWifiSmeP2pRole;
#define CSR_WIFI_SME_P2P_ROLE_NONE         ((CsrWifiSmeP2pRole) 0x00)
#define CSR_WIFI_SME_P2P_ROLE_STANDALONE   ((CsrWifiSmeP2pRole) 0x01)
#define CSR_WIFI_SME_P2P_ROLE_GO           ((CsrWifiSmeP2pRole) 0x02)
#define CSR_WIFI_SME_P2P_ROLE_CLI          ((CsrWifiSmeP2pRole) 0x03)

/*                                                                              

      
                       

             
                                                          

       
                                   
                            
                                                 
                                                               
                                                   
                                                  
                                              
                                        
                                              
                                             
                                           
                                                        
                                           
                                                                          
                                                
                                             
                                              
                                            
                                          
                                                                              
                                      
                                                              
                                                           
                                       
                                           
                                         
                                          

                                                                              */
typedef u8 CsrWifiSmeP2pStatus;
#define CSR_WIFI_SME_P2P_STATUS_SUCCESS                              ((CsrWifiSmeP2pStatus) 0x00)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_INFO_UNAVAILABLE                ((CsrWifiSmeP2pStatus) 0x01)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_INCOMPATIBLE_PARAM              ((CsrWifiSmeP2pStatus) 0x02)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_LIMIT_REACHED                   ((CsrWifiSmeP2pStatus) 0x03)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_INVALID_PARAM                   ((CsrWifiSmeP2pStatus) 0x04)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_ACCOMODATE                      ((CsrWifiSmeP2pStatus) 0x05)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_PREV_ERROR                      ((CsrWifiSmeP2pStatus) 0x06)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_COMMON_CHANNELS                 ((CsrWifiSmeP2pStatus) 0x07)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_UNKNOWN_GROUP                   ((CsrWifiSmeP2pStatus) 0x08)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_GO_INTENT                       ((CsrWifiSmeP2pStatus) 0x09)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_PROVISION_METHOD_INCOMPATIBLE   ((CsrWifiSmeP2pStatus) 0x0A)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_REJECT                          ((CsrWifiSmeP2pStatus) 0x0B)
#define CSR_WIFI_SME_P2P_STATUS_FAIL_RESERVED                        ((CsrWifiSmeP2pStatus) 0xFF)

/*                                                                              

      
                              

             
                                          
                                           

       
                                           
                                                                             
                                                                        
                                          
                                                                            
                                                                   

                                                                              */
typedef u8 CsrWifiSmePacketFilterMode;
#define CSR_WIFI_SME_PACKET_FILTER_MODE_OPT_OUT   ((CsrWifiSmePacketFilterMode) 0x00)
#define CSR_WIFI_SME_PACKET_FILTER_MODE_OPT_IN    ((CsrWifiSmePacketFilterMode) 0x03)

/*                                                                              

      
                            

             
                                                                    
                                                                

       
                                                                              
                                                            
                                                                              
                                                                                
                                                                              
                                                                                
                                                                         
                                                                 
                                                                                
                                                                           
                                                 

                                                                              */
typedef u8 CsrWifiSmePowerSaveLevel;
#define CSR_WIFI_SME_POWER_SAVE_LEVEL_LOW    ((CsrWifiSmePowerSaveLevel) 0x00)
#define CSR_WIFI_SME_POWER_SAVE_LEVEL_HIGH   ((CsrWifiSmePowerSaveLevel) 0x01)
#define CSR_WIFI_SME_POWER_SAVE_LEVEL_MED    ((CsrWifiSmePowerSaveLevel) 0x02)
#define CSR_WIFI_SME_POWER_SAVE_LEVEL_AUTO   ((CsrWifiSmePowerSaveLevel) 0x03)

/*                                                                              

      
                          

             
                      

       
                                                                
                                                           

                                                                              */
typedef u8 CsrWifiSmePreambleType;
#define CSR_WIFI_SME_USE_LONG_PREAMBLE    ((CsrWifiSmePreambleType) 0x00)
#define CSR_WIFI_SME_USE_SHORT_PREAMBLE   ((CsrWifiSmePreambleType) 0x01)

/*                                                                              

      
                     

             
                           

       
                                                                   
                                                                       
                                                                       

                                                                              */
typedef u8 CsrWifiSmeRadioIF;
#define CSR_WIFI_SME_RADIO_IF_GHZ_2_4   ((CsrWifiSmeRadioIF) 0x01)
#define CSR_WIFI_SME_RADIO_IF_GHZ_5_0   ((CsrWifiSmeRadioIF) 0x02)
#define CSR_WIFI_SME_RADIO_IF_BOTH      ((CsrWifiSmeRadioIF) 0x03)

/*                                                                              

      
                              

             
                                                                       

       
                                        
                                             
                                      
                                             
                                     
                                             
                                       
                                             
                                        
                                             
                                         
                                             
                                        
                                             
                                           
                                             
                                        
                                             
                                           
                                             
                                       
                                             

                                                                              */
typedef u8 CsrWifiSmeRegulatoryDomain;
#define CSR_WIFI_SME_REGULATORY_DOMAIN_OTHER      ((CsrWifiSmeRegulatoryDomain) 0x00)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_FCC        ((CsrWifiSmeRegulatoryDomain) 0x10)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_IC         ((CsrWifiSmeRegulatoryDomain) 0x20)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_ETSI       ((CsrWifiSmeRegulatoryDomain) 0x30)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_SPAIN      ((CsrWifiSmeRegulatoryDomain) 0x31)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_FRANCE     ((CsrWifiSmeRegulatoryDomain) 0x32)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_JAPAN      ((CsrWifiSmeRegulatoryDomain) 0x40)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_JAPANBIS   ((CsrWifiSmeRegulatoryDomain) 0x41)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_CHINA      ((CsrWifiSmeRegulatoryDomain) 0x50)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_CHINABIS   ((CsrWifiSmeRegulatoryDomain) 0x51)
#define CSR_WIFI_SME_REGULATORY_DOMAIN_NONE       ((CsrWifiSmeRegulatoryDomain) 0xFF)

/*                                                                              

      
                        

             
                                    

       
                                       
                                                                          
                                          
                                                       
                                            
                                                         
                                          
                                               

                                                                              */
typedef u8 CsrWifiSmeRoamReason;
#define CSR_WIFI_SME_ROAM_REASON_BEACONLOST        ((CsrWifiSmeRoamReason) 0x00)
#define CSR_WIFI_SME_ROAM_REASON_DISASSOCIATED     ((CsrWifiSmeRoamReason) 0x01)
#define CSR_WIFI_SME_ROAM_REASON_DEAUTHENTICATED   ((CsrWifiSmeRoamReason) 0x02)
#define CSR_WIFI_SME_ROAM_REASON_BETTERAPFOUND     ((CsrWifiSmeRoamReason) 0x03)

/*                                                                              

      
                      

             
                                               

       
                                                                                
                                                                   
                                                                               
                                                
                                                                           
                                             

                                                                              */
typedef u8 CsrWifiSmeScanType;
#define CSR_WIFI_SME_SCAN_TYPE_ALL       ((CsrWifiSmeScanType) 0x00)
#define CSR_WIFI_SME_SCAN_TYPE_ACTIVE    ((CsrWifiSmeScanType) 0x01)
#define CSR_WIFI_SME_SCAN_TYPE_PASSIVE   ((CsrWifiSmeScanType) 0x02)

/*                                                                              

      
                         

             
                                                              
                                                                             
                                                                           
                    

       
                                        
                                                                                
                                                                  
                                    
                                                                             
                                                                              
                              
                                      
                                                                             
                                                                              
                                      
                                        
                                                                             
                                                             

                                                                              */
typedef u8 CsrWifiSmeTrafficType;
#define CSR_WIFI_SME_TRAFFIC_TYPE_OCCASIONAL   ((CsrWifiSmeTrafficType) 0x00)
#define CSR_WIFI_SME_TRAFFIC_TYPE_BURSTY       ((CsrWifiSmeTrafficType) 0x01)
#define CSR_WIFI_SME_TRAFFIC_TYPE_PERIODIC     ((CsrWifiSmeTrafficType) 0x02)
#define CSR_WIFI_SME_TRAFFIC_TYPE_CONTINUOUS   ((CsrWifiSmeTrafficType) 0x03)

/*                                                                              

      
                       

             
                                                                              
                            

       
                                  
                                             
                                          
                                     
                                     
                                        

                                                                              */
typedef u8 CsrWifiSmeTspecCtrl;
#define CSR_WIFI_SME_TSPEC_CTRL_STRICT           ((CsrWifiSmeTspecCtrl) 0x01)
#define CSR_WIFI_SME_TSPEC_CTRL_CCX_SIGNALLING   ((CsrWifiSmeTspecCtrl) 0x02)
#define CSR_WIFI_SME_TSPEC_CTRL_CCX_VOICE        ((CsrWifiSmeTspecCtrl) 0x04)

/*                                                                              

      
                             

             
                                                         

       
                                     
                                                               
                                                     
                                                        
                                     
                                                
                                                      
                                                     
                                                      
                                                      
                                                    
                                                 
                                          
                                                 
                                                             
                                                 
                                     
                                                    
                                           
                                                                
                                                 
                                                           
                                                    
                                                                
                                       
                                                                       
                                                   
                                                              
                                                    
                                                 

                                                                              */
typedef u8 CsrWifiSmeTspecResultCode;
#define CSR_WIFI_SME_TSPEC_RESULT_SUCCESS                           ((CsrWifiSmeTspecResultCode) 0x00)
#define CSR_WIFI_SME_TSPEC_RESULT_UNSPECIFIED_QOS_FAILURE           ((CsrWifiSmeTspecResultCode) 0x01)
#define CSR_WIFI_SME_TSPEC_RESULT_FAILURE                           ((CsrWifiSmeTspecResultCode) 0x02)
#define CSR_WIFI_SME_TSPEC_RESULT_INVALID_TSPEC_PARAMETERS          ((CsrWifiSmeTspecResultCode) 0x05)
#define CSR_WIFI_SME_TSPEC_RESULT_INVALID_TCLAS_PARAMETERS          ((CsrWifiSmeTspecResultCode) 0x06)
#define CSR_WIFI_SME_TSPEC_RESULT_INSUFFICIENT_BANDWIDTH            ((CsrWifiSmeTspecResultCode) 0x07)
#define CSR_WIFI_SME_TSPEC_RESULT_WRONG_POLICY                      ((CsrWifiSmeTspecResultCode) 0x08)
#define CSR_WIFI_SME_TSPEC_RESULT_REJECTED_WITH_SUGGESTED_CHANGES   ((CsrWifiSmeTspecResultCode) 0x09)
#define CSR_WIFI_SME_TSPEC_RESULT_TIMEOUT                           ((CsrWifiSmeTspecResultCode) 0x0D)
#define CSR_WIFI_SME_TSPEC_RESULT_NOT_SUPPORTED                     ((CsrWifiSmeTspecResultCode) 0x0E)
#define CSR_WIFI_SME_TSPEC_RESULT_IE_LENGTH_INCORRECT               ((CsrWifiSmeTspecResultCode) 0x10)
#define CSR_WIFI_SME_TSPEC_RESULT_INVALID_TRANSACTION_ID            ((CsrWifiSmeTspecResultCode) 0x11)
#define CSR_WIFI_SME_TSPEC_RESULT_INSTALLED                         ((CsrWifiSmeTspecResultCode) 0x12)
#define CSR_WIFI_SME_TSPEC_RESULT_TID_ALREADY_INSTALLED             ((CsrWifiSmeTspecResultCode) 0x13)
#define CSR_WIFI_SME_TSPEC_RESULT_TSPEC_REMOTELY_DELETED            ((CsrWifiSmeTspecResultCode) 0x14)

/*                                                                              

      
                         

             
                                         

       
                                                                
                                                                       

                                                                              */
typedef u8 CsrWifiSmeWepAuthMode;
#define CSR_WIFI_SME_WEP_AUTH_MODE_OPEN     ((CsrWifiSmeWepAuthMode) 0x00)
#define CSR_WIFI_SME_WEP_AUTH_MODE_SHARED   ((CsrWifiSmeWepAuthMode) 0x01)

/*                                                                              

      
                               

             
                                          

       
                                      
                                      
                                       
                                       

                                                                              */
typedef u8 CsrWifiSmeWepCredentialType;
#define CSR_WIFI_SME_CREDENTIAL_TYPE_WEP64    ((CsrWifiSmeWepCredentialType) 0x00)
#define CSR_WIFI_SME_CREDENTIAL_TYPE_WEP128   ((CsrWifiSmeWepCredentialType) 0x01)

/*                                                                              

      
                     

             
                                                                        

       
                                                                 
                                                                  
                                                                          
                                                              
                                                                        
                                                              

                                                                              */
typedef u8 CsrWifiSmeWmmMode;
#define CSR_WIFI_SME_WMM_MODE_DISABLED     ((CsrWifiSmeWmmMode) 0x00)
#define CSR_WIFI_SME_WMM_MODE_AC_ENABLED   ((CsrWifiSmeWmmMode) 0x01)
#define CSR_WIFI_SME_WMM_MODE_PS_ENABLED   ((CsrWifiSmeWmmMode) 0x02)
#define CSR_WIFI_SME_WMM_MODE_SA_ENABLED   ((CsrWifiSmeWmmMode) 0x04)
#define CSR_WIFI_SME_WMM_MODE_ENABLED      ((CsrWifiSmeWmmMode) 0xFF)

/*                                                                              

      
                        

             
                                                                             
                                                                         
                                                                             
                                      
                                                                             
                                                                          

       
                                           
                                                           
                                   
                                                                             
                             
                                   
                                                                              
                             
                                   
                                                                            
                                    
                                   
                                                                             
                                    
                                           
                                                                             
                                        
                                            
                                                                             
                                        
                                           
                                                                             
                                        

                                                                              */
typedef u8 CsrWifiSmeWmmQosInfo;
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_MAX_SP_ALL    ((CsrWifiSmeWmmQosInfo) 0x00)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_VO            ((CsrWifiSmeWmmQosInfo) 0x01)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_VI            ((CsrWifiSmeWmmQosInfo) 0x02)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_BK            ((CsrWifiSmeWmmQosInfo) 0x04)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_BE            ((CsrWifiSmeWmmQosInfo) 0x08)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_MAX_SP_TWO    ((CsrWifiSmeWmmQosInfo) 0x20)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_MAX_SP_FOUR   ((CsrWifiSmeWmmQosInfo) 0x40)
#define CSR_WIFI_SME_WMM_QOS_INFO_AC_MAX_SP_SIX    ((CsrWifiSmeWmmQosInfo) 0x60)

/*                                                                              

      
                           

             
                                                 

       
                             
                          
                               
                            
                               
                                                                 
                               
                                                                 
                                 
                                                                  
                           
                        
                              
                           
                                   
                                                        
                                    
                                 
                                       
                                                               
                                        
                                                              

                                                                              */
typedef u16 CsrWifiSmeWpsConfigType;
#define CSR_WIFI_WPS_CONFIG_LABEL              ((CsrWifiSmeWpsConfigType) 0x0004)
#define CSR_WIFI_WPS_CONFIG_DISPLAY            ((CsrWifiSmeWpsConfigType) 0x0008)
#define CSR_WIFI_WPS_CONFIG_EXT_NFC            ((CsrWifiSmeWpsConfigType) 0x0010)
#define CSR_WIFI_WPS_CONFIG_INT_NFC            ((CsrWifiSmeWpsConfigType) 0x0020)
#define CSR_WIFI_WPS_CONFIG_NFC_IFACE          ((CsrWifiSmeWpsConfigType) 0x0040)
#define CSR_WIFI_WPS_CONFIG_PBC                ((CsrWifiSmeWpsConfigType) 0x0080)
#define CSR_WIFI_WPS_CONFIG_KEYPAD             ((CsrWifiSmeWpsConfigType) 0x0100)
#define CSR_WIFI_WPS_CONFIG_VIRTUAL_PBC        ((CsrWifiSmeWpsConfigType) 0x0280)
#define CSR_WIFI_WPS_CONFIG_PHYSICAL_PBC       ((CsrWifiSmeWpsConfigType) 0x0480)
#define CSR_WIFI_WPS_CONFIG_VIRTUAL_DISPLAY    ((CsrWifiSmeWpsConfigType) 0x2008)
#define CSR_WIFI_WPS_CONFIG_PHYSICAL_DISPLAY   ((CsrWifiSmeWpsConfigType) 0x4008)

/*                                                                              

      
                               

             
                            

       
                                         
                                 
                                      
                              
                                       
                                 
                                            
                                                   
                                    
                           
                                     
                            
                                       
                              
                                     
                            
                                        
                               
                                    
                            
                                       
                               
                                   
                          
                                    
                            

                                                                              */
typedef u8 CsrWifiSmeWpsDeviceCategory;
#define CSR_WIFI_SME_WPS_CATEGORY_UNSPECIFIED      ((CsrWifiSmeWpsDeviceCategory) 0x00)
#define CSR_WIFI_SME_WPS_CATEGORY_COMPUTER         ((CsrWifiSmeWpsDeviceCategory) 0x01)
#define CSR_WIFI_SME_WPS_CATEGORY_INPUT_DEV        ((CsrWifiSmeWpsDeviceCategory) 0x02)
#define CSR_WIFI_SME_WPS_CATEGORY_PRT_SCAN_FX_CP   ((CsrWifiSmeWpsDeviceCategory) 0x03)
#define CSR_WIFI_SME_WPS_CATEGORY_CAMERA           ((CsrWifiSmeWpsDeviceCategory) 0x04)
#define CSR_WIFI_SME_WPS_CATEGORY_STORAGE          ((CsrWifiSmeWpsDeviceCategory) 0x05)
#define CSR_WIFI_SME_WPS_CATEGORY_NET_INFRA        ((CsrWifiSmeWpsDeviceCategory) 0x06)
#define CSR_WIFI_SME_WPS_CATEGORY_DISPLAY          ((CsrWifiSmeWpsDeviceCategory) 0x07)
#define CSR_WIFI_SME_WPS_CATEGORY_MULTIMEDIA       ((CsrWifiSmeWpsDeviceCategory) 0x08)
#define CSR_WIFI_SME_WPS_CATEGORY_GAMING           ((CsrWifiSmeWpsDeviceCategory) 0x09)
#define CSR_WIFI_SME_WPS_CATEGORY_TELEPHONE        ((CsrWifiSmeWpsDeviceCategory) 0x0A)
#define CSR_WIFI_SME_WPS_CATEGORY_AUDIO            ((CsrWifiSmeWpsDeviceCategory) 0x0B)
#define CSR_WIFI_SME_WPS_CATEOARY_OTHERS           ((CsrWifiSmeWpsDeviceCategory) 0xFF)

/*                                                                              

      
                                  

             
                              

       
                                             
                              
                                             
                                               
                                            
                                            
                                              
                                   
                                             
                                         
                                                
                                         
                                              
                                 
                                            
                        
                                        
                         
                                                 
                              
                                             
                        
                                             
                          
                                           
                            
                                                 
                           
                                                  
                           
                                        
                        
                                               
                            
                                                 
                                                
                                              
                                 
                                                 
                                  
                                                 
                            
                                                 
                          
                                                  
                           
                                           
                                          
                                                  
                              
                                                 
                                 
                                                   
                                 
                                        
                        
                                                 
                                              
                                            
                               
                                          
                        
                                                   
                              
                                                 
                               
                                           
                                
                                                   
                             
                                                 
                                    
                                                       
                                     
                                                
                                                      
                                          
                           
                                               
                                                     
                                                 
                                              
                                                 
                             
                                                     
                                  
                                          
                               
                                          
                                                              
                                                 
                                            
                                                 
                                           
                                                 
                               
                                                   
                              
                                                  
                                   
                                           
                               
                                                  
                             
                                       
                                          
                                              
                                           
                                                      
                                 
                                                      
                                     
                                                  
                            
                                                    
                                     

                                                                              */
typedef u8 CsrWifiSmeWpsDeviceSubCategory;
#define CSR_WIFI_SME_WPS_SUB_CATEGORY_UNSPECIFIED             ((CsrWifiSmeWpsDeviceSubCategory) 0x00)
#define CSR_WIFI_SME_WPS_STORAGE_SUB_CATEGORY_NAS             ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_PSFC_SUB_CATEGORY_PRNTR              ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_TELEPHONE_SUB_CATEGORY_WM            ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_TUNER             ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_CAMERA_SUB_CATEGORY_DIG_STL          ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_NET_INFRA_SUB_CATEGORY_AP            ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_DISPLAY_SUB_CATEGORY_TV              ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_DAR                  ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_KEYBD         ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_PC             ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_GAMING_SUB_CATEGORY_XBOX             ((CsrWifiSmeWpsDeviceSubCategory) 0x01)
#define CSR_WIFI_SME_WPS_PSFC_SUB_CATEGORY_SCNR               ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_SERVER         ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_NET_INFRA_SUB_CATEGORY_ROUTER        ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_PVR                  ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_SPEAKER           ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_TELEPHONE_SUB_CATEGORY_FP_SM         ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_CAMERA_SUB_CATEGORY_VIDEO            ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_DISPLAY_SUB_CATEGORY_PIC_FRM         ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_GAMING_SUB_CATEGORY_XBOX_360         ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_MOUSE         ((CsrWifiSmeWpsDeviceSubCategory) 0x02)
#define CSR_WIFI_SME_WPS_NET_INFRA_SUB_CATEGORY_SWITCH        ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_PMP               ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_JOYSTK        ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_GAMING_SUB_CATEGORY_PLAY_STN         ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_MED_CENT       ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_MCX                  ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_TELEPHONE_SUB_CATEGORY_FP_DM         ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_CAMERA_SUB_CATEGORY_WEB              ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_PSFC_SUB_CATEGORY_FAX                ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_DISPLAY_SUB_CATEGORY_PROJECTOR       ((CsrWifiSmeWpsDeviceSubCategory) 0x03)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_TRKBL         ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_ST_BOX               ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_NET_INFRA_SUB_CATEGORY_GATEWAY       ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_CAMERA_SUB_CATEGORY_SECURITY         ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_ULTRA_MOB_PC   ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_GAMING_SUB_CATEGORY_CONSOLE          ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_PSFC_SUB_CATEGORY_CPR                ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_HEADSET           ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_TELEPHONE_SUB_CATEGORY_SP_SM         ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_DISPLAY_SUB_CATEGORY_MONITOR         ((CsrWifiSmeWpsDeviceSubCategory) 0x04)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_GAME_CTRL     ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_PSFC_SUB_CATEGORY_ALL                ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_MEDIA                ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_TELEPHONE_SUB_CATEGORY_SP_DM         ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_GAMING_SUB_CATEGORY_PORT_DEV         ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_HEADPHONE         ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_NOTEBOOK       ((CsrWifiSmeWpsDeviceSubCategory) 0x05)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_REMOTE        ((CsrWifiSmeWpsDeviceSubCategory) 0x06)
#define CSR_WIFI_SME_WPS_AUDIO_SUB_CATEGORY_MIC               ((CsrWifiSmeWpsDeviceSubCategory) 0x06)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_DESKTOP        ((CsrWifiSmeWpsDeviceSubCategory) 0x06)
#define CSR_WIFI_SME_WPS_MM_SUB_CATEGORY_VP                   ((CsrWifiSmeWpsDeviceSubCategory) 0x06)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_MID            ((CsrWifiSmeWpsDeviceSubCategory) 0x07)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_TOUCH_SCRN    ((CsrWifiSmeWpsDeviceSubCategory) 0x07)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_BIOMET_RDR    ((CsrWifiSmeWpsDeviceSubCategory) 0x08)
#define CSR_WIFI_SME_WPS_COMPUTER_SUB_CATEGORY_NETBOOK        ((CsrWifiSmeWpsDeviceSubCategory) 0x08)
#define CSR_WIFI_SME_WPS_INPUT_DEV_SUB_CATEGORY_BRCD_RDR      ((CsrWifiSmeWpsDeviceSubCategory) 0x09)

/*                                                                              

      
                     

             
                                                   

       
                                         
                                                                              
                                                           
                                                                           
                                             
                                                                      
                                         
                                                                               
                                                           

                                                                              */
typedef u16 CsrWifiSmeWpsDpid;
#define CSR_WIFI_SME_WPS_DPID_PIN         ((CsrWifiSmeWpsDpid) 0x0000)
#define CSR_WIFI_SME_WPS_DPID_USER        ((CsrWifiSmeWpsDpid) 0x0001)
#define CSR_WIFI_SME_WPS_DPID_MACHINE     ((CsrWifiSmeWpsDpid) 0x0002)
#define CSR_WIFI_SME_WPS_DPID_REKEY       ((CsrWifiSmeWpsDpid) 0x0003)
#define CSR_WIFI_SME_WPS_DPID_PBC         ((CsrWifiSmeWpsDpid) 0x0004)
#define CSR_WIFI_SME_WPS_DPID_REGISTRAR   ((CsrWifiSmeWpsDpid) 0x0005)

/*                                                                              

      
                             

             

       
                                                         
                                                         
                                                         

                                                                              */
typedef u8 CsrWifiSmeWpsRegistration;
#define CSR_WIFI_SME_WPS_REG_NOT_REQUIRED   ((CsrWifiSmeWpsRegistration) 0x00)
#define CSR_WIFI_SME_WPS_REG_REQUIRED       ((CsrWifiSmeWpsRegistration) 0x01)
#define CSR_WIFI_SME_WPS_REG_UNKNOWN        ((CsrWifiSmeWpsRegistration) 0x02)


/*                                                                              

      
                          

             
                                                                   

                                                                              */
typedef u16 CsrWifiSmeAuthModeMask;
/*                                                                              

      
                            

             
                                                                     

                                                                              */
typedef u16 CsrWifiSmeEncryptionMask;
/*                                                                              

      
                             

             
                                                                      

                                                                              */
typedef u32 CsrWifiSmeIndicationsMask;
/*                                                                              

      
                               

             
                                                                        

                                                                              */
typedef u8 CsrWifiSmeP2pCapabilityMask;
/*                                                                              

      
                                    

             
                                                                             

                                                                              */
typedef u8 CsrWifiSmeP2pGroupCapabilityMask;
/*                                                                              

      
                           

             
                                                                    

                                                                              */
typedef u8 CsrWifiSmeTspecCtrlMask;
/*                                                                              

      
                         

             
                                                                  

                                                                              */
typedef u8 CsrWifiSmeWmmModeMask;
/*                                                                              

      
                            

             
                                                                     

                                                                              */
typedef u8 CsrWifiSmeWmmQosInfoMask;
/*                                                                              

      
                               

             
                                                                        

                                                                              */
typedef u16 CsrWifiSmeWpsConfigTypeMask;


/*                                                                              

      
                         

             
                                                                 

         
                                                          
                                                           
                                                                                 
                                                   
                                                           
                                                                              

                                                                              */
typedef struct
{
    u16 atimWindowTu;
    u16 beaconPeriodTu;
    u16 joinOnlyAttempts;
    u16 joinAttemptIntervalMs;
} CsrWifiSmeAdHocConfig;

/*                                                                              

      
                                

             

         
                          
                          
                          

                                                                              */
typedef struct
{
    u8  listenChannel;
    u16 availabilityDuration;
    u16 avalabilityPeriod;
} CsrWifiSmeAvailabilityConfig;

/*                                                                              

      
                       

             
                                                                

         
                                 
                                 
                                 
                                 

                                                                              */
typedef struct
{
    u8 keepAliveTimeMs;
    u8  apRoamingEnabled;
    u8 measurementsMask;
    u8  ccxRadioMgtEnabled;
} CsrWifiSmeCcxConfig;

/*                                                                              

      
                        

             
                                             

         
                                                                              
                                                                           
                                                                          
                                                                            
                                                                             
                                                   
                                                                                
                                                                            
                                                                               
                                                     
                                                                              
                                                                    
                                                                              
                                                                        
                                                                                
                                         
                                                                              
                                         
                                                                             
                                                              
                                                                           
                                                              
                                                                            
                                                                        
                                                                               
                                                                   
                                                                               
                                                                             
                                                                           
                                           
                                                                                

                                                                              */
typedef struct
{
    u8   coexEnableSchemeManagement;
    u8   coexPeriodicWakeHost;
    u16 coexTrafficBurstyLatencyMs;
    u16 coexTrafficContinuousLatencyMs;
    u16 coexObexBlackoutDurationMs;
    u16 coexObexBlackoutPeriodMs;
    u16 coexA2dpBrBlackoutDurationMs;
    u16 coexA2dpBrBlackoutPeriodMs;
    u16 coexA2dpEdrBlackoutDurationMs;
    u16 coexA2dpEdrBlackoutPeriodMs;
    u16 coexPagingBlackoutDurationMs;
    u16 coexPagingBlackoutPeriodMs;
    u16 coexInquiryBlackoutDurationMs;
    u16 coexInquiryBlackoutPeriodMs;
} CsrWifiSmeCoexConfig;

/*                                                                              

      
                             

             
                                               
                                                                            

         
                   
                                                                               
                                                                    
                                                                            
                                                                                
                                                
                                                                        
                                                                               
                                                                             
                              
                   
                                                
                   
                                             
                           
                                             
                        
                                             
                            
                                             
                      
                                             
                        
                                             
                        
                                             
                    
                                             
                                 
                                             
                              
                                             
                         
                                             
                         
                                             
                              
                                             
                                    
                                             
                                       
                                             
                              
                                             
                                  
                                             
                                       
                                             
                                      
                                             
                             
                                             
                               
                                             
                             
                                             
                                   
                                             

                                                                              */
typedef struct
{
    u8  unifiTxDataRate;
    u8  unifiRxDataRate;
    u32 dot11RetryCount;
    u32 dot11MultipleRetryCount;
    u32 dot11AckFailureCount;
    u32 dot11FrameDuplicateCount;
    u32 dot11FcsErrorCount;
    u32 dot11RtsSuccessCount;
    u32 dot11RtsFailureCount;
    u32 dot11FailedCount;
    u32 dot11TransmittedFragmentCount;
    u32 dot11TransmittedFrameCount;
    u32 dot11WepExcludedCount;
    u32 dot11WepIcvErrorCount;
    u32 dot11WepUndecryptableCount;
    u32 dot11MulticastReceivedFrameCount;
    u32 dot11MulticastTransmittedFrameCount;
    u32 dot11ReceivedFragmentCount;
    u32 dot11Rsna4WayHandshakeFailures;
    u32 dot11RsnaTkipCounterMeasuresInvoked;
    u32 dot11RsnaStatsTkipLocalMicFailures;
    u32 dot11RsnaStatsTkipReplays;
    u32 dot11RsnaStatsTkipIcvErrors;
    u32 dot11RsnaStatsCcmpReplays;
    u32 dot11RsnaStatsCcmpDecryptErrors;
} CsrWifiSmeConnectionStats;

/*                                                                              

      
                       

             
                                                                 

         
                                     
                                                       

                                                                              */
typedef struct
{
    u16 length;
    u8 *data;
} CsrWifiSmeDataBlock;

/*                                                                              

      
                   

             
                                                                 

         
                                                                   

                                                                              */
typedef struct
{
    u8 empty;
} CsrWifiSmeEmpty;

/*                                                                              

      
                         

             
                                     

         
                                                                                
                   
                                                                     

                                                                              */
typedef struct
{
    s16 unifiRssi;
    s16 unifiSnr;
} CsrWifiSmeLinkQuality;

/*                                                                              

      
                       

             
                                               

         
                                                                            
                                                                          
                                                                              
                                                                                
                                                                        
                                                                 
                                                                      
                                                          
                                                          
                                                          

                                                                              */
typedef struct
{
    u8   unifiFixMaxTxDataRate;
    u8  unifiFixTxDataRate;
    u16 dot11RtsThreshold;
    u16 dot11FragmentationThreshold;
    u16 dot11CurrentTxPowerLevel;
} CsrWifiSmeMibConfig;

/*                                                                              

      
                                

             
                           

         
                          
                          
                          

                                                                              */
typedef struct
{
    u8  listenChannel;
    u16 availabilityDuration;
    u16 avalabilityPeriod;
} CsrWifiSmeP2pProfileIdentity;

/*                                                                              

      
                   

             
                                        

         
                           
                  

                                                                              */
typedef struct
{
    CsrWifiMacAddress bssid;
    u8          pmkid[16];
} CsrWifiSmePmkid;

/*                                                                              

      
                            

             
                                     

         
                                   
                                                                   

                                                                              */
typedef struct
{
    CsrWifiMacAddress bssid;
    u8           preAuthAllowed;
} CsrWifiSmePmkidCandidate;

/*                                                                              

      
                       

             
            
                               

         
                                              
                                                       

                                                                              */
typedef struct
{
    u8         pmkidsCount;
    CsrWifiSmePmkid *pmkids;
} CsrWifiSmePmkidList;

/*                                                                              

      
                                  

             
                              

         
                                         
                                                                       
                                     
                                                                   
                           
                                              
                      
                                                                       
                              

                                                                              */
typedef struct
{
    u8                    dot11MultiDomainCapabilityImplemented;
    u8                    dot11MultiDomainCapabilityEnabled;
    CsrWifiSmeRegulatoryDomain currentRegulatoryDomain;
    u8                   currentCountryCode[2];
} CsrWifiSmeRegulatoryDomainInfo;

/*                                                                              

      
                             

             
                                                                             

         
                                                                              
                                               
                                                                              
                                               
                                                                       
                                       
                                                                       
                                       

                                                                              */
typedef struct
{
    s16 rssiHighThreshold;
    s16 rssiLowThreshold;
    s16 snrHighThreshold;
    s16 snrLowThreshold;
} CsrWifiSmeRoamingBandData;

/*                                                                              

      
                            

             
                                                                

         
                                                                                
                                       
                                                                               
                                           
                                                                               
                                      
                                                                 
                                                                          
                                                                         
                                                                           
                                                                          
                                              
                                                                          
                                                                                
                                                
                                                                          
                                               

                                                                              */
typedef struct
{
    u16 intervalSeconds;
    u16 validitySeconds;
    u16 minActiveChannelTimeTu;
    u16 maxActiveChannelTimeTu;
    u16 minPassiveChannelTimeTu;
    u16 maxPassiveChannelTimeTu;
} CsrWifiSmeScanConfigData;

/*                                                                              

      
                     

             
                             

         
                      

                                                                              */
typedef struct
{
    u8 data[8];
} CsrWifiSmeTsfTime;

/*                                                                              

      
                      

             
                                                                             
            

         
                           
                                   
                                             
                                                               
                                                        
                                       
                                                      
                                    
                                                   

                                                                              */
typedef struct
{
    u32      chipId;
    u32      chipVersion;
    u32      firmwareBuild;
    u32      firmwarePatch;
    u32      firmwareHip;
    char *routerBuild;
    u32      routerHip;
    char *smeBuild;
    u32      smeHip;
} CsrWifiSmeVersions;

/*                                                                              

      
                         

             
                                         

         
                                                                               
                                     
                                                                               
                                    
                                                                           
                                                                  
                                                                          
                                                                          
                                                                          
                                                                              
                                         

                                                                              */
typedef struct
{
    u8  cwMin;
    u8  cwMax;
    u8  aifs;
    u16 txopLimit;
    u8   admissionControlMandatory;
} CsrWifiSmeWmmAcParams;

/*                                                                              

      
                           

             
                                              

         
                                               

                                                                              */
typedef struct
{
    u8 deviceDetails[8];
} CsrWifiSmeWpsDeviceType;

/*                                                                              

      
                                 

             

         
                
                

                                                                              */
typedef struct
{
    u8  spportWps;
    u8 deviceType;
} CsrWifiSmeWpsDeviceTypeCommon;

/*                                                                              

      
                     

             

         
                     
                     
                     

                                                                              */
typedef struct
{
    u16 version;
    u16 configMethods;
    u16 devicePassworId;
} CsrWifiSmeWpsInfo;

/*                                                                              

      
                               

             
                           

         
                                              
                                                                           

                                                                              */
typedef struct
{
    u8     cloakedSsidsCount;
    CsrWifiSsid *cloakedSsids;
} CsrWifiSmeCloakedSsidConfig;

/*                                                                              

      
                      

             
                                                 

         
                                                                     
                                                       
                                                                                
                                         
                                                                               
                                                        
                                                                              
                                                             
                                                                               
                                                                              
                                                                
                                                                             
                                                                        
                                         
                                                       
                                                                      
                                          

                                                                              */
typedef struct
{
    u8                  hasTrafficData;
    CsrWifiSmeTrafficType    currentTrafficType;
    u16                currentPeriodMs;
    CsrWifiSmePowerSaveLevel currentPowerSave;
    u16                currentCoexPeriodMs;
    u16                currentCoexLatencyMs;
    u8                  hasBtDevice;
    u32                currentBlackoutDurationUs;
    u32                currentBlackoutPeriodUs;
    CsrWifiSmeCoexScheme     currentCoexScheme;
} CsrWifiSmeCoexInfo;

/*                                                                              

      
                              

             
                                                                            

         
        
                                           
         
                                   
           
                                              
           
                                                  
               
                                                                               
                
                                                                               
                                          
                                                                 
                      
                                                                           
                                          
                                                                   
                                             
                                                                              
                                         
                                       
                                                                             
                         
              
                                                                           
                                
                                                   
                                                             
                                                                            
                                                  
                                                                   
                 
                                                                             
                                                          
                                                                               
                                                      
                                                                           
                                                                       
                                        
                
                                                                             
                                                          
                                                                               
                                                                              
                                                              

                                                                              */
typedef struct
{
    CsrWifiSsid                ssid;
    CsrWifiMacAddress          bssid;
    CsrWifiSmeBssType          bssType;
    CsrWifiSmeRadioIF          ifIndex;
    CsrWifiSme80211PrivacyMode privacyMode;
    CsrWifiSmeAuthModeMask     authModeMask;
    CsrWifiSmeEncryptionMask   encryptionModeMask;
    u16                  mlmeAssociateReqInformationElementsLength;
    u8                  *mlmeAssociateReqInformationElements;
    CsrWifiSmeWmmQosInfoMask   wmmQosInfo;
    u8                    adhocJoinOnly;
    u8                   adhocChannel;
} CsrWifiSmeConnectionConfig;

/*                                                                              

      
                            

             
                                                             

         
                                                        
                                                
                                                                        
                                                
                                                   
                                                                             
                                                                                
                                                                             
                                               
                                                               
                                                                
                                                                 
                                                                            
                                                                             
                                       
                                                                              
                                                                      
                                                           
                                                                             
                                               
                                                                      
                                                            
                                                                             
                                                
                                                                             
                                                                  
                                                                      
                                                                            
                                                                               
                                                                           
                                                                       
                                                                         
                                                      
                                                                              
                                         
                                                                              
                                               
                                                                      
                                                                          
                                                                             
                                                              
                                                                             
                                                                       
                                                                        
                                                       
                                                                            
                                                       
                                                                      
                                                                           
                                                                             
                                                               

                                                                              */
typedef struct
{
    CsrWifiSsid                ssid;
    CsrWifiMacAddress          bssid;
    CsrWifiSme80211NetworkType networkType80211;
    u8                   channelNumber;
    u16                  channelFrequency;
    CsrWifiSmeAuthMode         authMode;
    CsrWifiSmeEncryption       pairwiseCipher;
    CsrWifiSmeEncryption       groupCipher;
    CsrWifiSmeRadioIF          ifIndex;
    u16                  atimWindowTu;
    u16                  beaconPeriodTu;
    u8                    reassociation;
    u16                  beaconFrameLength;
    u8                  *beaconFrame;
    u16                  associationReqFrameLength;
    u8                  *associationReqFrame;
    u16                  associationRspFrameLength;
    u8                  *associationRspFrame;
    u16                  assocScanInfoElementsLength;
    u8                  *assocScanInfoElements;
    u16                  assocReqCapabilities;
    u16                  assocReqListenIntervalTu;
    CsrWifiMacAddress          assocReqApAddress;
    u16                  assocReqInfoElementsLength;
    u8                  *assocReqInfoElements;
    CsrWifiSmeIEEE80211Result  assocRspResult;
    u16                  assocRspCapabilityInfo;
    u16                  assocRspAssociationId;
    u16                  assocRspInfoElementsLength;
    u8                  *assocRspInfoElements;
} CsrWifiSmeConnectionInfo;

/*                                                                              

      
                          

             
                                            

         
                                                                               
                                     
                                                                               
                                                                              
                                          
                                                                           
                                                           

                                                                              */
typedef struct
{
    CsrWifiSme80211dTrustLevel        trustLevel;
    u8                          countryCode[2];
    CsrWifiSmeFirmwareDriverInterface firmwareDriverInterface;
    u8                           enableStrictDraftN;
} CsrWifiSmeDeviceConfig;

/*                                                                              

      
                        

             
                                    

         
                                                               
                                                                   
                                                      
                                                      
                                                               
                                                                 
                                                                           
                                                                      

                                                                              */
typedef struct
{
    CsrWifiMacAddress           deviceAddress;
    CsrWifiSmeWpsConfigTypeMask configMethods;
    CsrWifiSmeP2pCapabilityMask p2PDeviceCap;
    CsrWifiSmeWpsDeviceType     primDeviceType;
    u8                    secondaryDeviceTypeCount;
    CsrWifiSmeWpsDeviceType    *secDeviceType;
    u8                    deviceName[32];
    u8                    deviceNameLength;
} CsrWifiSmeDeviceInfo;

/*                                                                              

      
                              

             
                                         

         
                               
                               
                               
                               
                               
                               

                                                                              */
typedef struct
{
    CsrWifiMacAddress             p2pDeviceAddress;
    CsrWifiSmeWpsDeviceTypeCommon primaryDeviceType;
    u8                      secondaryDeviceTypesCount;
    u8                      secondaryDeviceTypes[10];
    u8                      deviceNameLength;
    u8                      deviceName[32];
} CsrWifiSmeDeviceInfoCommon;

/*                                                                              

      
                        

             
                                                                         
                                                       

         
                                                                             
                                                                               
                                          
                                                                            
                                                                            
                                                                           
                                                                            
                                      
                                                                                 
                                                                         
                                                                      
                                                                             
                                                              
                                                           

                                                                              */
typedef struct
{
    CsrWifiSmeHostPowerMode powerMode;
    u16               applicationDataPeriodMs;
} CsrWifiSmeHostConfig;

/*                                                                              

      
                 

             
                                                   

         
                                                                            
                                                              
                                                           
                                                                               
                                                                           
                              
                                                                               
                                                                   
                                                                      
                                                                              
                                                   
                                                
                                                              
                                           
                                            
                                              
                                                       

                                                                              */
typedef struct
{
    CsrWifiSmeKeyType keyType;
    u8          keyIndex;
    u8           wepTxKey;
    u16         keyRsc[8];
    u8           authenticator;
    CsrWifiMacAddress address;
    u8          keyLength;
    u8          key[32];
} CsrWifiSmeKey;

/*                                                                              

      
                               

             
                                    

         
                                                             
                                                  

                                                                              */
typedef struct
{
    CsrWifiMacAddress    p2PClientInterfaceAddress;
    CsrWifiSmeDeviceInfo clientDeviceInfo;
} CsrWifiSmeP2pClientInfoType;

/*                                                                              

      
                          

             
                                   

         
                                               
                                                 
                                                                        
                                                                              
                                                 

                                                                              */
typedef struct
{
    CsrWifiSmeP2pGroupCapabilityMask groupCapability;
    CsrWifiMacAddress                p2pDeviceAddress;
    u8                         p2pClientInfoCount;
    CsrWifiSmeP2pClientInfoType     *p2PClientInfo;
} CsrWifiSmeP2pGroupInfo;

/*                                                                              

      
                         

             
                                                                   

         
                                                    
                                                                         
                                                                           
                                                                  
                                                                            
                                                                             
                                   
                                       
                                       
                                       

                                                                              */
typedef struct
{
    CsrWifiSmePowerSaveLevel powerSaveLevel;
    u16                listenIntervalTu;
    u8                  rxDtims;
    CsrWifiSmeD3AutoScanMode d3AutoScanMode;
    u8                 clientTrafficWindow;
    u8                  opportunisticPowerSave;
    u8                  noticeOfAbsence;
} CsrWifiSmePowerConfig;

/*                                                                              

      
                           

             
                                                               

         
                                                                                
                                                               
                                                                                
                                                                
                                                                              
                                                                             
                                          
                                                                        
                                                                              
                                                                               
                                             
                                                                           
                                                                 
                                                                               
                                                             
                                                                               
                                                                

                                                                              */
typedef struct
{
    CsrWifiSmeRoamingBandData roamingBands[3];
    u8                   disableSmoothRoaming;
    u8                   disableRoamScans;
    u8                  reconnectLimit;
    u16                 reconnectLimitIntervalMs;
    CsrWifiSmeScanConfigData  roamScanCfg[3];
} CsrWifiSmeRoamingConfig;

/*                                                                              

      
                        

             
                                                                  

         
                                                      
                                                                          
                                                                     
                                                                               
                                                                                
                                                                          
                                                     
                                                                               
                                                                          
                                                     
                                                                             
                                                                              
                                                    
                                                                               
                                                                        
                                         
                                                                              
                                                                        
                                         
                                                                                
                                                                             
                                   
                                                                         
                                                                       
                                                 

                                                                              */
typedef struct
{
    CsrWifiSmeScanConfigData scanCfg[4];
    u8                  disableAutonomousScans;
    u16                maxResults;
    s8                  highRssiThreshold;
    s8                  lowRssiThreshold;
    s8                  deltaRssiThreshold;
    s8                  highSnrThreshold;
    s8                  lowSnrThreshold;
    s8                  deltaSnrThreshold;
    u16                passiveChannelListCount;
    u8                *passiveChannelList;
} CsrWifiSmeScanConfig;

/*                                                                              

      
                        

             
                                                             

         
                                                         
                                                 
                                                                             
                                                              
                                                                
                                                                 
                                                       
                                                                
                                                    
                                                           
                                                 
                                                                 
                                               
                                                                              
                                                                          
                                            
                                                                               
                                                                           
                                                                               
                                                             
                                                          
                                                              
                                                            
                                                                         
                                                                      
                                  
                                  
                                  
                                  

                                                                              */
typedef struct
{
    CsrWifiSsid              ssid;
    CsrWifiMacAddress        bssid;
    s16                 rssi;
    s16                 snr;
    CsrWifiSmeRadioIF        ifIndex;
    u16                beaconPeriodTu;
    CsrWifiSmeTsfTime        timeStamp;
    CsrWifiSmeTsfTime        localTime;
    u16                channelFrequency;
    u16                capabilityInformation;
    u8                 channelNumber;
    CsrWifiSmeBasicUsability usability;
    CsrWifiSmeBssType        bssType;
    u16                informationElementsLength;
    u8                *informationElements;
    CsrWifiSmeP2pRole        p2pDeviceRole;
    union {
        CsrWifiSmeEmpty        reservedCli;
        CsrWifiSmeP2pGroupInfo groupInfo;
        CsrWifiSmeEmpty        reservedNone;
        CsrWifiSmeDeviceInfo   standalonedevInfo;
    } deviceInfo;
} CsrWifiSmeScanResult;

/*                                                                              

      
                       

             
                                            

         
                                                                    
                                                                            
                                                          
                                                                                
                                                                        
                                                 
                                                                                
                                                          
                                                                               
                                                                            
                                                                             
                                                                               
                                                        

                                                                              */
typedef struct
{
    u8              connectionQualityRssiChangeTrigger;
    u8              connectionQualitySnrChangeTrigger;
    CsrWifiSmeWmmModeMask wmmModeMask;
    CsrWifiSmeRadioIF     ifIndex;
    u8               allowUnicastUseGroupCipher;
    u8               enableOpportunisticKeyCaching;
} CsrWifiSmeStaConfig;

/*                                                                              

      
                        

             
                                                                           
                      

         
                                                                                
                                                                       
                                                                               
                                                                              
                                 
                                            
                                            
                                            
                                            

                                                                              */
typedef struct
{
    CsrWifiSmeWepAuthMode wepAuthType;
    u8              selectedWepKey;
    u8              key1[13];
    u8              key2[13];
    u8              key3[13];
    u8              key4[13];
} CsrWifiSmeWep128Keys;

/*                                                                              

      
                       

             
                                                                           
                     

         
                                                                                
                                                                       
                                                                               
                                                                              
                                 
                                            
                                            
                                            
                                            

                                                                              */
typedef struct
{
    CsrWifiSmeWepAuthMode wepAuthType;
    u8              selectedWepKey;
    u8              key1[5];
    u8              key2[5];
    u8              key3[5];
    u8              key4[5];
} CsrWifiSmeWep64Keys;

/*                                                                              

      
                     

             
                                          

         
                                                              
                                                                            
                                                    
                              
                              

                                                                              */
typedef struct
{
    CsrWifiSmeWepCredentialType wepKeyType;
    union {
        CsrWifiSmeWep128Keys wep128Key;
        CsrWifiSmeWep64Keys  wep64Key;
    } wepCredentials;
} CsrWifiSmeWepAuth;

/*                                                                              

      
                       

             
                                         

         
                                                                                
                                     
                                    
                                                                           
                                             
                                                
                                               
                                              
                                            
                                          
                                              
                                           
                                                      
                                                               
                                                                 
                                                           
                                       
                                                                        

                                                                              */
typedef struct
{
    u8                    wpsVersion;
    u8                    uuid[16];
    u8                    deviceName[32];
    u8                    deviceNameLength;
    u8                    manufacturer[64];
    u8                    manufacturerLength;
    u8                    modelName[32];
    u8                    modelNameLength;
    u8                    modelNumber[32];
    u8                    modelNumberLength;
    u8                    serialNumber[32];
    CsrWifiSmeWpsDeviceType     primDeviceType;
    u8                    secondaryDeviceTypeCount;
    CsrWifiSmeWpsDeviceType    *secondaryDeviceType;
    CsrWifiSmeWpsConfigTypeMask configMethods;
    u8                    rfBands;
    u8                    osVersion[4];
} CsrWifiSmeWpsConfig;


/*            */
#define CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_SME_ACTIVATE_REQ                         ((CsrWifiSmePrim) (0x0000 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_ADHOC_CONFIG_GET_REQ                 ((CsrWifiSmePrim) (0x0001 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_ADHOC_CONFIG_SET_REQ                 ((CsrWifiSmePrim) (0x0002 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_BLACKLIST_REQ                        ((CsrWifiSmePrim) (0x0003 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CALIBRATION_DATA_GET_REQ             ((CsrWifiSmePrim) (0x0004 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CALIBRATION_DATA_SET_REQ             ((CsrWifiSmePrim) (0x0005 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CCX_CONFIG_GET_REQ                   ((CsrWifiSmePrim) (0x0006 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CCX_CONFIG_SET_REQ                   ((CsrWifiSmePrim) (0x0007 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_CONFIG_GET_REQ                  ((CsrWifiSmePrim) (0x0008 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_CONFIG_SET_REQ                  ((CsrWifiSmePrim) (0x0009 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_INFO_GET_REQ                    ((CsrWifiSmePrim) (0x000A + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECT_REQ                          ((CsrWifiSmePrim) (0x000B + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_CONFIG_GET_REQ            ((CsrWifiSmePrim) (0x000C + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_INFO_GET_REQ              ((CsrWifiSmePrim) (0x000D + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_STATS_GET_REQ             ((CsrWifiSmePrim) (0x000E + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_DEACTIVATE_REQ                       ((CsrWifiSmePrim) (0x000F + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_DISCONNECT_REQ                       ((CsrWifiSmePrim) (0x0010 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_EVENT_MASK_SET_REQ                   ((CsrWifiSmePrim) (0x0011 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_HOST_CONFIG_GET_REQ                  ((CsrWifiSmePrim) (0x0012 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_HOST_CONFIG_SET_REQ                  ((CsrWifiSmePrim) (0x0013 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_KEY_REQ                              ((CsrWifiSmePrim) (0x0014 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_LINK_QUALITY_GET_REQ                 ((CsrWifiSmePrim) (0x0015 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_CONFIG_GET_REQ                   ((CsrWifiSmePrim) (0x0016 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_CONFIG_SET_REQ                   ((CsrWifiSmePrim) (0x0017 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_GET_NEXT_REQ                     ((CsrWifiSmePrim) (0x0018 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_GET_REQ                          ((CsrWifiSmePrim) (0x0019 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_SET_REQ                          ((CsrWifiSmePrim) (0x001A + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_MULTICAST_ADDRESS_REQ                ((CsrWifiSmePrim) (0x001B + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_PACKET_FILTER_SET_REQ                ((CsrWifiSmePrim) (0x001C + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_PERMANENT_MAC_ADDRESS_GET_REQ        ((CsrWifiSmePrim) (0x001D + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_PMKID_REQ                            ((CsrWifiSmePrim) (0x001E + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_POWER_CONFIG_GET_REQ                 ((CsrWifiSmePrim) (0x001F + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_POWER_CONFIG_SET_REQ                 ((CsrWifiSmePrim) (0x0020 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_REGULATORY_DOMAIN_INFO_GET_REQ       ((CsrWifiSmePrim) (0x0021 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAMING_CONFIG_GET_REQ               ((CsrWifiSmePrim) (0x0022 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAMING_CONFIG_SET_REQ               ((CsrWifiSmePrim) (0x0023 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_CONFIG_GET_REQ                  ((CsrWifiSmePrim) (0x0024 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_CONFIG_SET_REQ                  ((CsrWifiSmePrim) (0x0025 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_FULL_REQ                        ((CsrWifiSmePrim) (0x0026 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_RESULTS_FLUSH_REQ               ((CsrWifiSmePrim) (0x0027 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_RESULTS_GET_REQ                 ((CsrWifiSmePrim) (0x0028 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_STA_CONFIG_GET_REQ               ((CsrWifiSmePrim) (0x0029 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_STA_CONFIG_SET_REQ               ((CsrWifiSmePrim) (0x002A + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_STATION_MAC_ADDRESS_GET_REQ          ((CsrWifiSmePrim) (0x002B + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_TSPEC_REQ                            ((CsrWifiSmePrim) (0x002C + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_VERSIONS_GET_REQ                     ((CsrWifiSmePrim) (0x002D + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_FLIGHTMODE_REQ                  ((CsrWifiSmePrim) (0x002E + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_OFF_REQ                         ((CsrWifiSmePrim) (0x002F + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_ON_REQ                          ((CsrWifiSmePrim) (0x0030 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CLOAKED_SSIDS_SET_REQ                ((CsrWifiSmePrim) (0x0031 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_CLOAKED_SSIDS_GET_REQ                ((CsrWifiSmePrim) (0x0032 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_COMMON_CONFIG_GET_REQ            ((CsrWifiSmePrim) (0x0033 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_COMMON_CONFIG_SET_REQ            ((CsrWifiSmePrim) (0x0034 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_INTERFACE_CAPABILITY_GET_REQ         ((CsrWifiSmePrim) (0x0035 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_WPS_CONFIGURATION_REQ                ((CsrWifiSmePrim) (0x0036 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_SET_REQ                              ((CsrWifiSmePrim) (0x0037 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_SME_PRIM_DOWNSTREAM_HIGHEST           (0x0037 + CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_SME_ACTIVATE_CFM                         ((CsrWifiSmePrim)(0x0000 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ADHOC_CONFIG_GET_CFM                 ((CsrWifiSmePrim)(0x0001 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ADHOC_CONFIG_SET_CFM                 ((CsrWifiSmePrim)(0x0002 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ASSOCIATION_COMPLETE_IND             ((CsrWifiSmePrim)(0x0003 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ASSOCIATION_START_IND                ((CsrWifiSmePrim)(0x0004 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_BLACKLIST_CFM                        ((CsrWifiSmePrim)(0x0005 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CALIBRATION_DATA_GET_CFM             ((CsrWifiSmePrim)(0x0006 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CALIBRATION_DATA_SET_CFM             ((CsrWifiSmePrim)(0x0007 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CCX_CONFIG_GET_CFM                   ((CsrWifiSmePrim)(0x0008 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CCX_CONFIG_SET_CFM                   ((CsrWifiSmePrim)(0x0009 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_CONFIG_GET_CFM                  ((CsrWifiSmePrim)(0x000A + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_CONFIG_SET_CFM                  ((CsrWifiSmePrim)(0x000B + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_COEX_INFO_GET_CFM                    ((CsrWifiSmePrim)(0x000C + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECT_CFM                          ((CsrWifiSmePrim)(0x000D + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_CONFIG_GET_CFM            ((CsrWifiSmePrim)(0x000E + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_INFO_GET_CFM              ((CsrWifiSmePrim)(0x000F + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_QUALITY_IND               ((CsrWifiSmePrim)(0x0010 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CONNECTION_STATS_GET_CFM             ((CsrWifiSmePrim)(0x0011 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_DEACTIVATE_CFM                       ((CsrWifiSmePrim)(0x0012 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_DISCONNECT_CFM                       ((CsrWifiSmePrim)(0x0013 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_EVENT_MASK_SET_CFM                   ((CsrWifiSmePrim)(0x0014 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_HOST_CONFIG_GET_CFM                  ((CsrWifiSmePrim)(0x0015 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_HOST_CONFIG_SET_CFM                  ((CsrWifiSmePrim)(0x0016 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_IBSS_STATION_IND                     ((CsrWifiSmePrim)(0x0017 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_KEY_CFM                              ((CsrWifiSmePrim)(0x0018 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_LINK_QUALITY_GET_CFM                 ((CsrWifiSmePrim)(0x0019 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MEDIA_STATUS_IND                     ((CsrWifiSmePrim)(0x001A + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_CONFIG_GET_CFM                   ((CsrWifiSmePrim)(0x001B + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_CONFIG_SET_CFM                   ((CsrWifiSmePrim)(0x001C + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_GET_CFM                          ((CsrWifiSmePrim)(0x001D + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_GET_NEXT_CFM                     ((CsrWifiSmePrim)(0x001E + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIB_SET_CFM                          ((CsrWifiSmePrim)(0x001F + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MIC_FAILURE_IND                      ((CsrWifiSmePrim)(0x0020 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_MULTICAST_ADDRESS_CFM                ((CsrWifiSmePrim)(0x0021 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_PACKET_FILTER_SET_CFM                ((CsrWifiSmePrim)(0x0022 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_PERMANENT_MAC_ADDRESS_GET_CFM        ((CsrWifiSmePrim)(0x0023 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_PMKID_CANDIDATE_LIST_IND             ((CsrWifiSmePrim)(0x0024 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_PMKID_CFM                            ((CsrWifiSmePrim)(0x0025 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_POWER_CONFIG_GET_CFM                 ((CsrWifiSmePrim)(0x0026 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_POWER_CONFIG_SET_CFM                 ((CsrWifiSmePrim)(0x0027 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_REGULATORY_DOMAIN_INFO_GET_CFM       ((CsrWifiSmePrim)(0x0028 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAM_COMPLETE_IND                    ((CsrWifiSmePrim)(0x0029 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAM_START_IND                       ((CsrWifiSmePrim)(0x002A + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAMING_CONFIG_GET_CFM               ((CsrWifiSmePrim)(0x002B + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ROAMING_CONFIG_SET_CFM               ((CsrWifiSmePrim)(0x002C + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_CONFIG_GET_CFM                  ((CsrWifiSmePrim)(0x002D + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_CONFIG_SET_CFM                  ((CsrWifiSmePrim)(0x002E + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_FULL_CFM                        ((CsrWifiSmePrim)(0x002F + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_RESULT_IND                      ((CsrWifiSmePrim)(0x0030 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_RESULTS_FLUSH_CFM               ((CsrWifiSmePrim)(0x0031 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SCAN_RESULTS_GET_CFM                 ((CsrWifiSmePrim)(0x0032 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_STA_CONFIG_GET_CFM               ((CsrWifiSmePrim)(0x0033 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_STA_CONFIG_SET_CFM               ((CsrWifiSmePrim)(0x0034 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_STATION_MAC_ADDRESS_GET_CFM          ((CsrWifiSmePrim)(0x0035 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_TSPEC_IND                            ((CsrWifiSmePrim)(0x0036 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_TSPEC_CFM                            ((CsrWifiSmePrim)(0x0037 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_VERSIONS_GET_CFM                     ((CsrWifiSmePrim)(0x0038 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_FLIGHTMODE_CFM                  ((CsrWifiSmePrim)(0x0039 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_OFF_IND                         ((CsrWifiSmePrim)(0x003A + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_OFF_CFM                         ((CsrWifiSmePrim)(0x003B + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_ON_CFM                          ((CsrWifiSmePrim)(0x003C + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CLOAKED_SSIDS_SET_CFM                ((CsrWifiSmePrim)(0x003D + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CLOAKED_SSIDS_GET_CFM                ((CsrWifiSmePrim)(0x003E + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WIFI_ON_IND                          ((CsrWifiSmePrim)(0x003F + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_COMMON_CONFIG_GET_CFM            ((CsrWifiSmePrim)(0x0040 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_SME_COMMON_CONFIG_SET_CFM            ((CsrWifiSmePrim)(0x0041 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_INTERFACE_CAPABILITY_GET_CFM         ((CsrWifiSmePrim)(0x0042 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_ERROR_IND                            ((CsrWifiSmePrim)(0x0043 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_INFO_IND                             ((CsrWifiSmePrim)(0x0044 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_CORE_DUMP_IND                        ((CsrWifiSmePrim)(0x0045 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AMP_STATUS_CHANGE_IND                ((CsrWifiSmePrim)(0x0046 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_WPS_CONFIGURATION_CFM                ((CsrWifiSmePrim)(0x0047 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_SME_PRIM_UPSTREAM_HIGHEST             (0x0047 + CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_SME_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_SME_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_SME_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_SME_PRIM_UPSTREAM_COUNT               (CSR_WIFI_SME_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_SME_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                         

             
                                                     
                                                                         

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeActivateReq;

/*                                                                              

      
                               

             
                                                               

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeAdhocConfigGetReq;

/*                                                                              

      
                               

             
                                                               

         
                                                                  
                                                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrWifiSmeAdHocConfig adHocConfig;
} CsrWifiSmeAdhocConfigSetReq;

/*                                                                              

      
                          

             
                                                                             
                                                                         
                                                                            
                                                          
                                                                            
                                                                            
                                                                       
               

         
                                                                      
                                                                             
                                                                               
                                                                             
                               
                                                               
                                                                                
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrWifiSmeListAction action;
    u8             setAddressCount;
    CsrWifiMacAddress   *setAddresses;
} CsrWifiSmeBlacklistReq;

/*                                                                              

      
                                   

             
                                                              

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeCalibrationDataGetReq;

/*                                                                              

      
                                   

             
                                                         
                                                                      
                                                

         
                                                                            
                                                                    
                                           
                                                                               
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       calibrationDataLength;
    u8       *calibrationData;
} CsrWifiSmeCalibrationDataSetReq;

/*                                                                              

      
                             

             
                                                             
                            

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeCcxConfigGetReq;

/*                                                                              

      
                             

             
                                                             
                            

         
                                                                   
                                                                          
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    u16           interfaceTag;
    CsrWifiSmeCcxConfig ccxConfig;
} CsrWifiSmeCcxConfigSetReq;

/*                                                                              

      
                              

             
                                                              

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeCoexConfigGetReq;

/*                                                                              

      
                              

             
                                                              

         
                                                                 
                                                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrWifiSmeCoexConfig coexConfig;
} CsrWifiSmeCoexConfigSetReq;

/*                                                                              

      
                            

             
                                                            

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeCoexInfoGetReq;

/*                                                                              

      
                        

             
                                                                      
                                                                       
            
                                                                            
                                                                          
                                                                             
                            
                                                                           
                                                                           
                                                                  
                                                                              
                                                                                 
                                                              
                                                                        
                                                                              
                                                                             
                                                                             
                                                                                
                                                                              
                             
                                                                               
                                                                             
                                                                               
                                                                             
                              
                                                                          
                                                                          
                                                                          
                                                                          
              
                                                                         
                                                                         
                                   
                                                                             
                                                                          
            
                                                                         

         
                                                                       
                                                                              
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    CsrWifiSmeConnectionConfig connectionConfig;
} CsrWifiSmeConnectReq;

/*                                                                              

      
                                    

             
                                                                    

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeConnectionConfigGetReq;

/*                                                                              

      
                                  

             
                                                                  

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeConnectionInfoGetReq;

/*                                                                              

      
                                   

             
                                                                   

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeConnectionStatsGetReq;

/*                                                                              

      
                           

             
                                                       

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeDeactivateReq;

/*                                                                              

      
                           

             
                                                                            
                             

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeDisconnectReq;

/*                                                                              

      
                             

             
                                                                         
                                                                            
                                                                       
                                                                         

         
                                                              
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    CsrWifiSmeIndicationsMask indMask;
} CsrWifiSmeEventMaskSetReq;

/*                                                                              

      
                              

             
                                                              

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeHostConfigGetReq;

/*                                                                              

      
                              

             
                                                              

         
                                                                   
                                                                          
                                                                             
                                                                               
                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrWifiSmeHostConfig hostConfig;
} CsrWifiSmeHostConfigSetReq;

/*                                                                              

      
                    

             
                                                                          
                                                         
                                                                           
                                           

         
                                                                   
                                                                          
                                                                                
                                                                
                                                                      
                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrWifiSmeListAction action;
    CsrWifiSmeKey        key;
} CsrWifiSmeKeyReq;

/*                                                                              

      
                               

             
                                                               

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeLinkQualityGetReq;

/*                                                                              

      
                             

             
                                                             

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeMibConfigGetReq;

/*                                                                              

      
                             

             
                                                             

         
                                                                
                                                                              
                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    CsrWifiSmeMibConfig mibConfig;
} CsrWifiSmeMibConfigSetReq;

/*                                                                              

      
                           

             
                                                                        
                                                       

         
                                                                         
                                               
                                                                          
                                                                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       mibAttributeLength;
    u8       *mibAttribute;
} CsrWifiSmeMibGetNextReq;

/*                                                                              

      
                       

             
                                                                            
                       

         
                                                                         
                                               
                                                                            
                                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       mibAttributeLength;
    u8       *mibAttribute;
} CsrWifiSmeMibGetReq;

/*                                                                              

      
                       

             
                                                                            
                                                                             
                                     
                                                                   
                                                                    
                                     
                                                                           
                          
                                                                            
                 

         
                                                                         
                                               
                                                                            
                                                                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       mibAttributeLength;
    u8       *mibAttribute;
} CsrWifiSmeMibSetReq;

/*                                                                              

      
                                 

             
                                                                        
                                                                             
                                                                        
                                                                            
           

         
                                                                        
                                                                               
                                                                       
                                                                             
                                      
                                                                       
                                                                          
                                                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrWifiSmeListAction action;
    u8             setAddressesCount;
    CsrWifiMacAddress   *setAddresses;
} CsrWifiSmeMulticastAddressReq;

/*                                                                              

      
                                

             
                                                                            
                                                                           
                                                                        
         
                                                                            
                                                       
                                                                             
                                                               
                     
                                                                         
                                              
                                                                 

         
                                                                       
                                                                              
                                                     
                                                                        
                                                                               
                                                                      
                                                                 
                                                                  
                                                
                                                                               
                                          
                                                                             
                                                                                 
                                                                                 
                                
                                                                             
                                                                                  
                                                                                    
                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    u16                  filterLength;
    u8                  *filter;
    CsrWifiSmePacketFilterMode mode;
    CsrWifiIp4Address          arpFilterAddress;
} CsrWifiSmePacketFilterSetReq;

/*                                                                              

      
                                       

             
                                                             

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmePermanentMacAddressGetReq;

/*                                                                              

      
                      

             
                                                                       
                                    
                                                           
                                                                         
                                                               
                                  
                                                                           
                              

         
                                                                     
                                                                            
                                                                              
                                                                        
                                                             
                                                                               
                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrWifiSmeListAction action;
    u8             setPmkidsCount;
    CsrWifiSmePmkid     *setPmkids;
} CsrWifiSmePmkidReq;

/*                                                                              

      
                               

             
                                                               

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmePowerConfigGetReq;

/*                                                                              

      
                               

             
                                                               

         
                                                                  
                                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrWifiSmePowerConfig powerConfig;
} CsrWifiSmePowerConfigSetReq;

/*                                                                              

      
                                        

             
                                                                        

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeRegulatoryDomainInfoGetReq;

/*                                                                              

      
                                 

             
                                                                 

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeRoamingConfigGetReq;

/*                                                                              

      
                                 

             
                                                                 

         
                                                                    
                                                                           
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrWifiSmeRoamingConfig roamingConfig;
} CsrWifiSmeRoamingConfigSetReq;

/*                                                                              

      
                              

             
                                                              

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeScanConfigGetReq;

/*                                                                              

      
                              

             
                                                              
                                                                           
                               
                                                                           
                                             
                                           
                                  
                                                                              
                                                                
                                                                            
                                                                

         
                                                                 
                                                                                
                                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrWifiSmeScanConfig scanConfig;
} CsrWifiSmeScanConfigSetReq;

/*                                                                              

      
                         

             
                                                                            
              
                                                                           
                             
                                                                             
                                                                            
              

         
                                                                       
                                                
                                                                             
                                                                 
                                      
                                                                                
                                             
                                                                             
                                                                            
                                                                                
                                                                             
                                              
                                              
                                                   
                                                                          
                                                                           
                                         
                                                                                
                               
                                                                            
                                              
                                                                                
                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent    common;
    u8           ssidCount;
    CsrWifiSsid       *ssid;
    CsrWifiMacAddress  bssid;
    u8            forceScan;
    CsrWifiSmeBssType  bssType;
    CsrWifiSmeScanType scanType;
    u16          channelListCount;
    u8          *channelList;
    u16          probeIeLength;
    u8          *probeIe;
} CsrWifiSmeScanFullReq;

/*                                                                              

      
                                 

             
                                                                          
                                                                            
                      
                                                                       
                             
                                                                          
                                
                                                                          
                                                                             
                                                                

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeScanResultsFlushReq;

/*                                                                              

      
                               

             
                                                                         
                                                                    
                                                                  

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeScanResultsGetReq;

/*                                                                              

      
                                

             
                                                                

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeSmeStaConfigGetReq;

/*                                                                              

      
                                

             
                                                             

         
                                                                   
                                                                          
                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    u16           interfaceTag;
    CsrWifiSmeStaConfig smeConfig;
} CsrWifiSmeSmeStaConfigSetReq;

/*                                                                              

      
                                     

             
                                                                           
            

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeStationMacAddressGetReq;

/*                                                                              

      
                      

             
                                                                          
                  
                                                                     
                                           
                                                                         
                                                 
                                                                             
                                                                    
                                                                            
            
                                                                     
                                                                        
          
                                                                             
                                                                             
                                                                 

         
                                                                    
                                                                           
                                                                                 
                                                                          
                                                                           
                          
                                                                           
                                     
                                                           
                                                                  
                                           
                                        
                                                         
                                        
                                                                            
                                                                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrWifiSmeListAction    action;
    u32               transactionId;
    u8                 strict;
    CsrWifiSmeTspecCtrlMask ctrlMask;
    u16               tspecLength;
    u8               *tspec;
    u16               tclasLength;
    u8               *tclas;
} CsrWifiSmeTspecReq;

/*                                                                              

      
                            

             
                                                            

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeVersionsGetReq;

/*                                                                              

      
                               

             
                                                                          
                                                                          
                              
                                                                 
                                                                            
                                                                            
                                                                     
                                                           
                                                                          
                                                                            
                                                                            
                                                                            
            
                                                                        
                                                                      
                                                                              
                                                                     
                                                   

         
                                                                    
                                                               
                                                                             
                                                                              
                                               
                                                                          
                                                                           
                                                                                
                                                                             
                                               
                                                                              
                           
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrWifiMacAddress    address;
    u16            mibFilesCount;
    CsrWifiSmeDataBlock *mibFiles;
} CsrWifiSmeWifiFlightmodeReq;

/*                                                                              

      
                        

             
                                                                         
                                                     

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeWifiOffReq;

/*                                                                              

      
                       

             
                                                                        
               
                                                                        
                                                                            
                             
                                                                       
                                                   
                                                                         
                               
                                                                          
                                     

         
                                                                    
                                                               
                                                                             
                                                                              
                                              
                                                                          
                                                                           
                                                                                
                                                                             
                                               
                                                                              
                           
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrWifiMacAddress    address;
    u16            mibFilesCount;
    CsrWifiSmeDataBlock *mibFiles;
} CsrWifiSmeWifiOnReq;

/*                                                                              

      
                                

             
                                                                             
             
                                                                             
                                                                           
                                                                            
                                                        

         
                                                                   
                                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent             common;
    CsrWifiSmeCloakedSsidConfig cloakedSsids;
} CsrWifiSmeCloakedSsidsSetReq;

/*                                                                              

      
                                

             
                                                                

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeCloakedSsidsGetReq;

/*                                                                              

      
                                   

             
                                                              

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeSmeCommonConfigGetReq;

/*                                                                              

      
                                   

             
                                                    

         
                                                                   
                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent        common;
    CsrWifiSmeDeviceConfig deviceConfig;
} CsrWifiSmeSmeCommonConfigSetReq;

/*                                                                              

      
                                       

             
                                                                    
                                            

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiSmeInterfaceCapabilityGetReq;

/*                                                                              

      
                                 

             
                                                                             
                                               

         
                                                                
                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    CsrWifiSmeWpsConfig wpsConfig;
} CsrWifiSmeWpsConfigurationReq;

/*                                                                              

      
                    

             
                                                                          
                                    
                                                                           
                

         
                                                                 
                                                                   
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u32       dataLength;
    u8       *data;
} CsrWifiSmeSetReq;

/*                                                                              

      
                         

             
                                                                 

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeActivateCfm;

/*                                                                              

      
                               

             
                                                     

         
                                                                  
                                                   
                                                                         
                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrResult             status;
    CsrWifiSmeAdHocConfig adHocConfig;
} CsrWifiSmeAdhocConfigGetCfm;

/*                                                                              

      
                               

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeAdhocConfigSetCfm;

/*                                                                              

      
                                    

             
                                                                             
                                                                           
                                                         
                                                                           
                                   

         
                                                                     
                                                                            
                                                                    
                                                                 
                                                 
                                                                                
                                                                     
                                                 
                                                                               
                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrResult                 status;
    CsrWifiSmeConnectionInfo  connectionInfo;
    CsrWifiSmeIEEE80211Reason deauthReason;
} CsrWifiSmeAssociationCompleteInd;

/*                                                                              

      
                                 

             
                                                                             
                                                                     

         
                                                                   
                                                                          
                                                   
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrWifiMacAddress address;
    CsrWifiSsid       ssid;
} CsrWifiSmeAssociationStartInd;

/*                                                                              

      
                          

             
                                                                         
                                                                        
                            

         
                                                                      
                                                                             
                                                       
                                           
                                                                  
                                                   
                                                               
                                                                                
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrResult            status;
    CsrWifiSmeListAction action;
    u8             getAddressCount;
    CsrWifiMacAddress   *getAddresses;
} CsrWifiSmeBlacklistCfm;

/*                                                                              

      
                                   

             
                                                     

         
                                                                            
                                                             
                                                                    
                                           
                                                                               
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
    u16       calibrationDataLength;
    u8       *calibrationData;
} CsrWifiSmeCalibrationDataGetCfm;

/*                                                                              

      
                                   

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeCalibrationDataSetCfm;

/*                                                                              

      
                             

             
                                                     

         
                                                                   
                                                                          
                                                    
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    u16           interfaceTag;
    CsrResult           status;
    CsrWifiSmeCcxConfig ccxConfig;
} CsrWifiSmeCcxConfigGetCfm;

/*                                                                              

      
                             

             
                                                     

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeCcxConfigSetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                                 
                                                  
                                                                         
                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrResult            status;
    CsrWifiSmeCoexConfig coexConfig;
} CsrWifiSmeCoexConfigGetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeCoexConfigSetCfm;

/*                                                                              

      
                            

             
                                                     

         
                                                               
                                                
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent    common;
    CsrResult          status;
    CsrWifiSmeCoexInfo coexInfo;
} CsrWifiSmeCoexInfoGetCfm;

/*                                                                              

      
                        

             
                                                                            
                                 

         
                                                                   
                                                                          
                                                     
                                                                            
                                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeConnectCfm;

/*                                                                              

      
                                    

             
                                                     

         
                                                                       
                                                                              
                                                        
                                                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    CsrResult                  status;
    CsrWifiSmeConnectionConfig connectionConfig;
} CsrWifiSmeConnectionConfigGetCfm;

/*                                                                              

      
                                  

             
                                                     

         
                                                                     
                                                                            
                                                      
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent          common;
    u16                interfaceTag;
    CsrResult                status;
    CsrWifiSmeConnectionInfo connectionInfo;
} CsrWifiSmeConnectionInfoGetCfm;

/*                                                                              

      
                                  

             
                                                                         
                                                                   
                                                                 
                                                                             
                                                             
                                    
                                                                            
            

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    CsrWifiSmeLinkQuality linkQuality;
} CsrWifiSmeConnectionQualityInd;

/*                                                                              

      
                                   

             
                                                     

         
                                                                      
                                                                             
                                                       
                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrResult                 status;
    CsrWifiSmeConnectionStats connectionStats;
} CsrWifiSmeConnectionStatsGetCfm;

/*                                                                              

      
                           

             
                                                                   
                                                                          
                                              

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeDeactivateCfm;

/*                                                                              

      
                           

             
                                                                      
                                                                 
                                                                            
                              

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeDisconnectCfm;

/*                                                                              

      
                             

             
                                                                   
              

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeEventMaskSetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                                   
                                                                          
                                                    
                                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrResult            status;
    CsrWifiSmeHostConfig hostConfig;
} CsrWifiSmeHostConfigGetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeHostConfigSetCfm;

/*                                                                              

      
                            

             
                                                                             
                            

         
                                                                  
                                                                    
                                                                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    CsrWifiMacAddress address;
    u8           isconnected;
} CsrWifiSmeIbssStationInd;

/*                                                                              

      
                    

             
                                                                   
              

         
                                                                     
                                                                            
                                                      
                                          
                                                  
                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrResult            status;
    CsrWifiSmeListAction action;
    CsrWifiSmeKeyType    keyType;
    CsrWifiMacAddress    peerMacAddress;
} CsrWifiSmeKeyCfm;

/*                                                                              

      
                               

             
                                                     

         
                                                                   
                                                                          
                                                    
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    CsrResult             status;
    CsrWifiSmeLinkQuality linkQuality;
} CsrWifiSmeLinkQualityGetCfm;

/*                                                                              

      
                            

             
                                                                         
                                                                             
               

         
                                                                     
                                                                            
                                               
                                                                          
                                                         
                                                                                
                                                                          
                                                            
                                                                                
                                   
                                                                          
                                                            
                                                                              
                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiSmeMediaStatus     mediaStatus;
    CsrWifiSmeConnectionInfo  connectionInfo;
    CsrWifiSmeIEEE80211Reason disassocReason;
    CsrWifiSmeIEEE80211Reason deauthReason;
} CsrWifiSmeMediaStatusInd;

/*                                                                              

      
                             

             
                                                     

         
                                                                
                                                 
                                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    CsrResult           status;
    CsrWifiSmeMibConfig mibConfig;
} CsrWifiSmeMibConfigGetCfm;

/*                                                                              

      
                             

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeMibConfigSetCfm;

/*                                                                              

      
                       

             
                                                                             

         
                                                                         
                                                          
                                               
                                                                            
                                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
    u16       mibAttributeLength;
    u8       *mibAttribute;
} CsrWifiSmeMibGetCfm;

/*                                                                              

      
                           

             
                                                                     
                                                                       
                                                                      

         
                                                                         
                                                          
                                               
                                                                          
                                                                         
                                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
    u16       mibAttributeLength;
    u8       *mibAttribute;
} CsrWifiSmeMibGetNextCfm;

/*                                                                              

      
                       

             
                                                                          

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeMibSetCfm;

/*                                                                              

      
                           

             
                                                                         
                                                                

         
                                                                    
                                                                           
                                                                         
                           
                                                                             
                               
                                                                              
                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    u8           secondFailure;
    u16         count;
    CsrWifiMacAddress address;
    CsrWifiSmeKeyType keyType;
} CsrWifiSmeMicFailureInd;

/*                                                                              

      
                                 

             
                                                                          
                                                                         

         
                                                                        
                                                                               
                                                         
                                             
                                                                    
                                                     
                                                                       
                                                                          
                                                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrResult            status;
    CsrWifiSmeListAction action;
    u8             getAddressesCount;
    CsrWifiMacAddress   *getAddresses;
} CsrWifiSmeMulticastAddressCfm;

/*                                                                              

      
                                

             
                                                                          

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmePacketFilterSetCfm;

/*                                                                              

      
                                       

             
                                                     

         
                                                                          
                                                           
                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    CsrResult         status;
    CsrWifiMacAddress permanentMacAddress;
} CsrWifiSmePermanentMacAddressGetCfm;

/*                                                                              

      
                                   

             
                                                                             
                                                                         
                    

         
                                                                           
                                                                        
                                    
                                                              
                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    u8                  pmkidCandidatesCount;
    CsrWifiSmePmkidCandidate *pmkidCandidates;
} CsrWifiSmePmkidCandidateListInd;

/*                                                                              

      
                      

             
                                                                          
                                                                 

         
                                                                     
                                                                            
                                                      
                                          
                                                                 
                                                  
                                                             
                                                                               
                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    u16            interfaceTag;
    CsrResult            status;
    CsrWifiSmeListAction action;
    u8             getPmkidsCount;
    CsrWifiSmePmkid     *getPmkids;
} CsrWifiSmePmkidCfm;

/*                                                                              

      
                               

             
                                                     

         
                                                                  
                                                   
                                                                               
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrResult             status;
    CsrWifiSmePowerConfig powerConfig;
} CsrWifiSmePowerConfigGetCfm;

/*                                                                              

      
                               

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmePowerConfigSetCfm;

/*                                                                              

      
                                        

             
                                                     

         
                                                                 
                                                  
                                                                           
                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrResult                      status;
    CsrWifiSmeRegulatoryDomainInfo regDomInfo;
} CsrWifiSmeRegulatoryDomainInfoGetCfm;

/*                                                                              

      
                             

             
                                                                             
                                                                             
                                                                       
                                                            

         
                                                                   
                                                                          
                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeRoamCompleteInd;

/*                                                                              

      
                          

             
                                                                             
                                                              
                                                                          
                                                                            
                                                                      
                                                                            
                                                                          
                                             
                                                                      
                                                                        
           
                                                                             
                                                                           
            
                                                                      
                                                          
                                          
                                                                          
                                                                           

         
                                                                   
                                                                          
                                                                          
                                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiSmeRoamReason      roamReason;
    CsrWifiSmeIEEE80211Reason reason80211;
} CsrWifiSmeRoamStartInd;

/*                                                                              

      
                                 

             
                                                     

         
                                                                    
                                                                           
                                                     
                                                                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrResult               status;
    CsrWifiSmeRoamingConfig roamingConfig;
} CsrWifiSmeRoamingConfigGetCfm;

/*                                                                              

      
                                 

             
                                                                 

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeRoamingConfigSetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                                 
                                                  
                                                                           
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrResult            status;
    CsrWifiSmeScanConfig scanConfig;
} CsrWifiSmeScanConfigGetCfm;

/*                                                                              

      
                              

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeScanConfigSetCfm;

/*                                                                              

      
                         

             
                                                                   
              

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeScanFullCfm;

/*                                                                              

      
                           

             
                                                                         
                                                                        

         
                                                             
                                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent      common;
    CsrWifiSmeScanResult result;
} CsrWifiSmeScanResultInd;

/*                                                                              

      
                                 

             
                                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeScanResultsFlushCfm;

/*                                                                              

      
                               

             
                                                                            

         
                                                                       
                                                        
                                             
                                                                
                                                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrResult             status;
    u16             scanResultsCount;
    CsrWifiSmeScanResult *scanResults;
} CsrWifiSmeScanResultsGetCfm;

/*                                                                              

      
                                

             
                                                     

         
                                                                   
                                                                          
                                                    
                                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    u16           interfaceTag;
    CsrResult           status;
    CsrWifiSmeStaConfig smeConfig;
} CsrWifiSmeSmeStaConfigGetCfm;

/*                                                                              

      
                                

             
                                                     

         
                                                                   
                                                                          
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeSmeStaConfigSetCfm;

/*                                                                              

      
                                     

             
                                                     

         
                                                                        
                                                         
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    CsrResult         status;
    CsrWifiMacAddress stationMacAddress[2];
} CsrWifiSmeStationMacAddressGetCfm;

/*                                                                              

      
                      

             
                                                                            
                                                        

         
                                                                      
                                                                             
                                                                             
                            
                                                                         
                             
                                          
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    u32                 transactionId;
    CsrWifiSmeTspecResultCode tspecResultCode;
    u16                 tspecLength;
    u8                 *tspec;
} CsrWifiSmeTspecInd;

/*                                                                              

      
                      

             
                                                                           
            

         
                                                                      
                                                                             
                                                       
                                                                             
                            
                                                                            
                                          
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrResult                 status;
    u32                 transactionId;
    CsrWifiSmeTspecResultCode tspecResultCode;
    u16                 tspecLength;
    u8                 *tspec;
} CsrWifiSmeTspecCfm;

/*                                                                              

      
                            

             
                                                     

         
                                                               
                                                
                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent    common;
    CsrResult          status;
    CsrWifiSmeVersions versions;
} CsrWifiSmeVersionsGetCfm;

/*                                                                              

      
                               

             
                                                                           
                                                                         
                                                              
                             

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeWifiFlightmodeCfm;

/*                                                                              

      
                        

             
                                                                         
                                                           

         
                                                             
                                                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent             common;
    CsrWifiSmeControlIndication reason;
} CsrWifiSmeWifiOffInd;

/*                                                                              

      
                        

             
                                                                            
                                                                     
                                      
                                                              
                                                   

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeWifiOffCfm;

/*                                                                              

      
                       

             
                                                                           
                                                           

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeWifiOnCfm;

/*                                                                              

      
                                

             
                                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeCloakedSsidsSetCfm;

/*                                                                              

      
                                

             
                                                     

         
                                                                   
                                                    
                                                                                
                                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent             common;
    CsrResult                   status;
    CsrWifiSmeCloakedSsidConfig cloakedSsids;
} CsrWifiSmeCloakedSsidsGetCfm;

/*                                                                              

      
                       

             
                                                                             
                                                        

         
                                                              
                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    CsrWifiMacAddress address;
} CsrWifiSmeWifiOnInd;

/*                                                                              

      
                                   

             
                                                     

         
                                                                   
                                                    
                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent        common;
    CsrResult              status;
    CsrWifiSmeDeviceConfig deviceConfig;
} CsrWifiSmeSmeCommonConfigGetCfm;

/*                                                                              

      
                                   

             
                                     

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeSmeCommonConfigSetCfm;

/*                                                                              

      
                                       

             
                                                     

         
                                                                    
                                         
                                                      
                                                                                
                              
                                                                  
                                         
                              
                              
                             
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
    u16       numInterfaces;
    u8        capBitmap[2];
} CsrWifiSmeInterfaceCapabilityGetCfm;

/*                                                                              

      
                      

             
                                                                 

         
                                                                   
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    char  *errorMessage;
} CsrWifiSmeErrorInd;

/*                                                                              

      
                     

             
                                                                             
                                              

         
                                                                  
                                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    char  *infoMessage;
} CsrWifiSmeInfoInd;

/*                                                                              

      
                         

             
                                                                             
                                      
                                                                     
               
                                                                             
                                       

         
                                                                 
                                                                   
                                                                            
                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u32       dataLength;
    u8       *data;
} CsrWifiSmeCoreDumpInd;

/*                                                                              

      
                                

             
                                         

         
                                                                   
                                                               
                                                                     
                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    u16           interfaceTag;
    CsrWifiSmeAmpStatus ampStatus;
} CsrWifiSmeAmpStatusChangeInd;

/*                                                                              

      
                                 

             
            

         
                                                             
                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeWpsConfigurationCfm;

#endif /*                       */

