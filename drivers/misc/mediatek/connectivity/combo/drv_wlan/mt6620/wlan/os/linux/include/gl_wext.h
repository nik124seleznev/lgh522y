/*
                                                                                            
*/

/*                   
                                                                              
*/



/*
                    
  
                   
                                                                             
                                                   
  
                           
                                                                                                             
    
  
                           
                                                                                                             
    
  
                           
       
                                                                                                                           
                                                                            
  
                   
       
                                                                            
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                   
                                                          
                                                        
  
                           
                                             
                                
                                                              
                                                                                                                                    
                                                              
                                                                                                           
                                                              
                                   
                                                             
                                           
                                     
                                 
                                                             
                         
                                                             
                                          
                                                             
                                           
                                                             
                                  
                                                             
                                                     
                                                             
                            
*/

#ifndef _GL_WEXT_H
#define _GL_WEXT_H

#ifdef WIRELESS_EXT
/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#define KILO          1000
#define RATE_5_5M     11	/*      */

/*                                                                              
                                                 
                                                                                
*/
typedef struct _PARAM_FIXED_IEs {
	UINT_8 aucTimestamp[8];
	UINT_16 u2BeaconInterval;
	UINT_16 u2Capabilities;
} PARAM_FIXED_IEs;

typedef struct _PARAM_VARIABLE_IE_T {
	UINT_8 ucElementID;
	UINT_8 ucLength;
	UINT_8 aucData[1];
} PARAM_VARIABLE_IE_T, *P_PARAM_VARIABLE_IE_T;


#if WIRELESS_EXT < 18

#define SIOCSIWMLME 0x8B16	/*                                             */
/*                                              */
#define IW_MLME_DEAUTH      0
#define IW_MLME_DISASSOC    1

/*                          */
struct iw_mlme {
	__u16 cmd;		/*             */
	__u16 reason_code;
	struct sockaddr addr;
};

#define SIOCSIWAUTH 0x8B32	/*                                */
#define SIOCGIWAUTH 0x8B33	/*                                */
/*                                               */
#define IW_AUTH_INDEX       0x0FFF
#define IW_AUTH_FLAGS       0xF000
/*                                               
                                                                         
                                                                    
                                */
#define IW_AUTH_WPA_VERSION             0
#define IW_AUTH_CIPHER_PAIRWISE         1
#define IW_AUTH_CIPHER_GROUP            2
#define IW_AUTH_KEY_MGMT                3
#define IW_AUTH_TKIP_COUNTERMEASURES    4
#define IW_AUTH_DROP_UNENCRYPTED        5
#define IW_AUTH_80211_AUTH_ALG          6
#define IW_AUTH_WPA_ENABLED             7
#define IW_AUTH_RX_UNENCRYPTED_EAPOL    8
#define IW_AUTH_ROAMING_CONTROL         9
#define IW_AUTH_PRIVACY_INVOKED        10
#if CFG_SUPPORT_802_11W
#define IW_AUTH_MFP                    12

#define IW_AUTH_MFP_DISABLED    0	/*              */
#define IW_AUTH_MFP_OPTIONAL    1	/*              */
#define IW_AUTH_MFP_REQUIRED    2	/*              */
#endif

/*                                        */
#define IW_AUTH_WPA_VERSION_DISABLED    0x00000001
#define IW_AUTH_WPA_VERSION_WPA         0x00000002
#define IW_AUTH_WPA_VERSION_WPA2        0x00000004

/*                                                                     */
#define IW_AUTH_CIPHER_NONE     0x00000001
#define IW_AUTH_CIPHER_WEP40    0x00000002
#define IW_AUTH_CIPHER_TKIP     0x00000004
#define IW_AUTH_CIPHER_CCMP     0x00000008
#define IW_AUTH_CIPHER_WEP104   0x00000010

/*                                     */
#define IW_AUTH_KEY_MGMT_802_1X     1
#define IW_AUTH_KEY_MGMT_PSK        2
#define IW_AUTH_KEY_MGMT_WPA_NONE   4

/*                                           */
#define IW_AUTH_ALG_OPEN_SYSTEM 0x00000001
#define IW_AUTH_ALG_SHARED_KEY  0x00000002
#define IW_AUTH_ALG_LEAP        0x00000004

/*                                */
#define IW_AUTH_ROAMING_ENABLE  0	/*                               */
#define IW_AUTH_ROAMING_DISABLE 1	/*                                    
                */

#define SIOCSIWENCODEEXT 0x8B34	/*                           */
#define SIOCGIWENCODEEXT 0x8B35	/*                           */
/*                              */
#define IW_ENCODE_SEQ_MAX_SIZE  8
/*                            */
#define IW_ENCODE_ALG_NONE  0
#define IW_ENCODE_ALG_WEP   1
#define IW_ENCODE_ALG_TKIP  2
#define IW_ENCODE_ALG_CCMP  3
#if CFG_SUPPORT_802_11W
#define IW_ENCODE_ALG_AES_CMAC  5
#endif

/*                                  */
#define IW_ENCODE_EXT_TX_SEQ_VALID  0x00000001
#define IW_ENCODE_EXT_RX_SEQ_VALID  0x00000002
#define IW_ENCODE_EXT_GROUP_KEY     0x00000004
#define IW_ENCODE_EXT_SET_TX_KEY    0x00000008

struct iw_encode_ext {
	__u32 ext_flags;	/*                   */
	__u8 tx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*             */
	__u8 rx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*             */
	struct sockaddr addr;	/*                                            
                                            
                         */
	__u16 alg;		/*                   */
	__u16 key_len;
	__u8 key[0];
};

#define SIOCSIWPMKSA        0x8B36	/*                       */
#define IW_PMKSA_ADD        1
#define IW_PMKSA_REMOVE     2
#define IW_PMKSA_FLUSH      3

#define IW_PMKID_LEN        16

struct iw_pmksa {
	__u32 cmd;		/*              */
	struct sockaddr bssid;
	__u8 pmkid[IW_PMKID_LEN];
};

#define IWEVGENIE   0x8C05	/*                               
                                           
                                       
                                     
                                      
                         */
#define IWEVMICHAELMICFAILURE 0x8C06	/*                    
                                     
      */
#define IWEVASSOCREQIE  0x8C07	/*                                     
                                      
                                           
                                    
                                 
                                         
                                 
                                   
                    */
#define IWEVASSOCRESPIE 0x8C08	/*                            
                                         
                                         
                                    
                                 
                                    
                    */
#define IWEVPMKIDCAND   0x8C09	/*                        
                         
                              */

#endif				/*                   */

#if WIRELESS_EXT < 17
/*                                       */
#define IW_QUAL_QUAL_UPDATED    0x1	/*                                   */
#define IW_QUAL_LEVEL_UPDATED   0x2
#define IW_QUAL_NOISE_UPDATED   0x4
#define IW_QUAL_QUAL_INVALID    0x10	/*                              */
#define IW_QUAL_LEVEL_INVALID   0x20
#define IW_QUAL_NOISE_INVALID   0x40
#endif

enum {
	IEEE80211_FILTER_TYPE_BEACON = 1 << 0,
	IEEE80211_FILTER_TYPE_PROBE_REQ = 1 << 1,
	IEEE80211_FILTER_TYPE_PROBE_RESP = 1 << 2,
	IEEE80211_FILTER_TYPE_ASSOC_REQ = 1 << 3,
	IEEE80211_FILTER_TYPE_ASSOC_RESP = 1 << 4,
	IEEE80211_FILTER_TYPE_AUTH = 1 << 5,
	IEEE80211_FILTER_TYPE_DEAUTH = 1 << 6,
	IEEE80211_FILTER_TYPE_DISASSOC = 1 << 7,
	IEEE80211_FILTER_TYPE_ALL = 0xFF	/*                                     */
};

#if CFG_SUPPORT_WAPI
#define IW_AUTH_WAPI_ENABLED     0x20
#define IW_ENCODE_ALG_SMS4  0x20
#endif

#if CFG_SUPPORT_WAPI		/*          */
#define IW_AUTH_KEY_MGMT_WAPI_PSK   3
#define IW_AUTH_KEY_MGMT_WAPI_CERT  4
#endif
#define IW_AUTH_KEY_MGMT_WPS  5

#if CFG_SUPPORT_802_11W
#define IW_AUTH_KEY_MGMT_802_1X_SHA256 7
#define IW_AUTH_KEY_MGMT_PSK_SHA256 8
#endif


/*                                                                              
                                                  
                                                                                
*/
extern const struct iw_handler_def wext_handler_def;

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/
/*                             */
int wext_support_ioctl(IN struct net_device *prDev, IN struct ifreq *prIfReq, IN int i4Cmd);

int
wext_set_rate(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo, IN struct iw_param *prRate, IN char *pcExtra);

void
wext_indicate_wext_event(IN P_GLUE_INFO_T prGlueInfo,
			 IN unsigned int u4Cmd,
			 IN unsigned char *pucData, IN unsigned int u4DataLen);

struct iw_statistics *wext_get_wireless_stats(struct net_device *prDev);

int wext_get_priv(IN struct net_device *prNetDev, IN struct ifreq *prIfReq);

BOOLEAN
wextSrchDesiredWPAIE(IN PUINT_8 pucIEStart,
		     IN INT_32 i4TotalIeLen,
		     IN UINT_8 ucDesiredElemId, OUT PUINT_8 * ppucDesiredIE);

#if CFG_SUPPORT_WPS
BOOLEAN
wextSrchDesiredWPSIE(IN PUINT_8 pucIEStart,
		     IN INT_32 i4TotalIeLen,
		     IN UINT_8 ucDesiredElemId, OUT PUINT_8 * ppucDesiredIE);
#endif

/*                                                                              
                                                
                                                                                
*/

#endif				/*              */

#endif				/*            */
