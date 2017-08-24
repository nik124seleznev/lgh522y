/*
                                                                                    
*/

/*                 
                                                                               
*/



/*
                    
  
                        
       
                           
                                                      
  
                   
                                                                                        
                                                                              
  
                   
                                                                                                     
                                                                        
  
                   
                                                                                                     
                                                                                   
  
                   
                                                                                                 
                                                         
                                                                
  
                   
                                                                             
                                                   
  
                          
                                                                                              
                                            
  
                         
                                                      
                          
  
                   
                                                                                                                                   
                                                    
  
                           
       
                                                                                                                  
  
                         
                                                                                                                    
                                  
  
                        
                                                           
                           
  
                           
                                                                         
                          
  
                      
                                                              
                                                                                                                                       
  
                   
                                                                                
                                                                                             
  
                           
                                                                                
    
  
                      
                                                                           
                                                  
  
                   
                                                                                                                         
                                                                                                     
  
                          
                                                                              
                                   
  
                   
                                                                                                 
                                                     
  
                        
                                                       
                     
  
                        
                                                       
                    
  
                        
                                                       
                                       
  
                           
       
                                                                                                                           
                                                                            
  
                      
                                                                                          
                                                                       
  
                   
                                                                                                                                                   
                                                                                 
                               
  
                   
                                                                                                                                                   
                                                                                                             
  
                      
                                                                                          
                                                          
  
                          
                                                               
                                                        
  
                          
                                                               
                                                                
  
                           
                                                                                             
                     
  
                           
       
                                                           
  
                      
                                                                                                        
                      
  
                      
                                                                                                        
                                              
                                          
  
                   
                                                                                                                  
    
  
                   
                                                                                        
                                          
  
                           
                                                                                                                             
                                                                                        
  
                           
                                                                                                           
                                                         
  
                   
                                                                            
                                    
  
                   
                                                                                                
                                    
  
                   
       
                                                                            
  
                         
                                                                         
                                  
  
                   
       
                                                       
  
                   
       
                                                             
  
                   
       
                                                
  
                           
       
                          
  
                           
       
                                                    
                                              
  
                           
       
                             
                                                                
  
                           
       
                             
                             
                                                       
  
                           
  
                                
  
                           
  
                             
  
                           
  
                          
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                           
                                             
                                         
  
                           
                                             
                                                 
  
                           
                                             
  
                                                     
                                                                        
  
                           
                                             
                                
  
                           
                                             
                                
  
                           
                                             
                       
  
                           
                                             
                                                                                                  
                  
  
                           
                                             
                                        
  
                   
                                                          
                                                                                
                                                  
  
                           
                                             
                 
  
                           
                                             
                               
  
                           
                                             
                                                
  
                   
                                                          
                                                        
  
                           
                                             
                                    
  
                           
                                             
                                
                                                              
                                                                                                                                         
                                                              
                           
                                                              
                                                                                                           
                                                              
                                 
                                                              
                                           
                                     
                                 
                                                              
                         
                                                              
                                                       
                                                              
                             
                                                              
                                                              
                                                              
                            
                                                              
                                                                   
                                                              
                         
                                                              
                                                                   
                                                              
                                            
                                                              
                  
                                                              
                                           
                                                              
                                
                                                              
                                                              
                                                       
  
                                                              
                                                        
                            
                                                              
               
                                                              
                                                     
                                                              
                                                              
                                                                                                            
                                                              
                  
                                                              
            
                                                              
                          
  
                                                              
                         
                                                              
                            
                                                             
                
                                                             
                                  
*/

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#include "gl_os.h"

#include "config.h"
#include "wlan_oid.h"

#include "gl_wext.h"
#include "gl_wext_priv.h"

#include "precomp.h"

#if CFG_SUPPORT_WAPI
#include "gl_sec.h"
#endif

/*                                      */
#ifdef WIRELESS_EXT

/*                                                                              
                                                
                                                                                
*/
const long channel_freq[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};

#define     MAP_CHANNEL_ID_TO_KHZ(ch, khz)  {               \
		switch (ch)                                 \
		{                                           \
		    case 1:     khz = 2412000;   break;     \
		    case 2:     khz = 2417000;   break;     \
		    case 3:     khz = 2422000;   break;     \
		    case 4:     khz = 2427000;   break;     \
		    case 5:     khz = 2432000;   break;     \
		    case 6:     khz = 2437000;   break;     \
		    case 7:     khz = 2442000;   break;     \
		    case 8:     khz = 2447000;   break;     \
		    case 9:     khz = 2452000;   break;     \
		    case 10:    khz = 2457000;   break;     \
		    case 11:    khz = 2462000;   break;     \
		    case 12:    khz = 2467000;   break;     \
		    case 13:    khz = 2472000;   break;     \
		    case 14:    khz = 2484000;   break;     \
		    case 36:  /*      */  khz = 5180000;   break;     \
		    case 40:  /*      */  khz = 5200000;   break;     \
		    case 44:  /*      */  khz = 5220000;   break;     \
		    case 48:  /*      */  khz = 5240000;   break;     \
		    case 52:  /*      */  khz = 5260000;   break;     \
		    case 56:  /*      */  khz = 5280000;   break;     \
		    case 60:  /*      */  khz = 5300000;   break;     \
		    case 64:  /*      */  khz = 5320000;   break;     \
		    case 149: /*      */  khz = 5745000;   break;     \
		    case 153: /*      */  khz = 5765000;   break;     \
		    case 157: /*      */  khz = 5785000;   break;     \
		    case 161: /*      */  khz = 5805000;   break;     \
		    case 165: /*      */  khz = 5825000;   break;     \
		    case 100: /*           */  khz = 5500000;   break;     \
		    case 104: /*           */  khz = 5520000;   break;     \
		    case 108: /*           */  khz = 5540000;   break;     \
		    case 112: /*           */  khz = 5560000;   break;     \
		    case 116: /*           */  khz = 5580000;   break;     \
		    case 120: /*           */  khz = 5600000;   break;     \
		    case 124: /*           */  khz = 5620000;   break;     \
		    case 128: /*           */  khz = 5640000;   break;     \
		    case 132: /*           */  khz = 5660000;   break;     \
		    case 136: /*           */  khz = 5680000;   break;     \
		    case 140: /*           */  khz = 5700000;   break;     \
		    case 34:  /*            */   khz = 5170000;   break;   \
		    case 38:  /*            */   khz = 5190000;   break;   \
		    case 42:  /*            */   khz = 5210000;   break;   \
		    case 46:  /*            */   khz = 5230000;   break;   \
		    case 184: /*       */   khz = 4920000;   break;   \
		    case 188: /*       */   khz = 4940000;   break;   \
		    case 192: /*       */   khz = 4960000;   break;   \
		    case 196: /*       */   khz = 4980000;   break;   \
		    case 208: /*                  */   khz = 5040000;   break;   \
		    case 212: /*                  */   khz = 5060000;   break;   \
		    case 216: /*                  */   khz = 5080000;   break;   \
		    default:    khz = 2412000;   break;     \
		}                                           \
	    }


#define NUM_CHANNELS (sizeof(channel_freq) / sizeof(channel_freq[0]))

#define MAX_SSID_LEN    32


/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
/*                                              */
static const struct iw_priv_args rIwPrivTable[] = {
	{IOCTL_SET_INT, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, ""},
	{IOCTL_GET_INT, 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, ""},
	{IOCTL_SET_INT, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, 0, ""},
	{IOCTL_GET_INT, 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, ""},
	{IOCTL_SET_INT, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, ""},

	{IOCTL_GET_INT, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, ""},
	{IOCTL_GET_INT, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, ""},

	{IOCTL_SET_INTS, IW_PRIV_TYPE_INT | 4, 0, ""},
	{IOCTL_GET_INT, 0, IW_PRIV_TYPE_INT | 50, ""},

	/*                               */
	{IOCTL_SET_STRUCT, 256, 0, ""},
	{IOCTL_GET_STRUCT, 0, 256, ""},

	/*                       */
#if 0
	{PRIV_CMD_REG_DOMAIN, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_reg_domain"},
	{PRIV_CMD_REG_DOMAIN, 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_reg_domain"},
#endif

#if CFG_TCP_IP_CHKSUM_OFFLOAD
	{PRIV_CMD_CSUM_OFFLOAD, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_tcp_csum"},
#endif				/*                           */

	{PRIV_CMD_POWER_MODE, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_power_mode"},
	{PRIV_CMD_POWER_MODE, 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_power_mode"},

	{PRIV_CMD_WMM_PS, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3, 0, "set_wmm_ps"},

	{PRIV_CMD_TEST_MODE, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_test_mode"},
	{PRIV_CMD_TEST_CMD, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "set_test_cmd"},
	{PRIV_CMD_TEST_CMD, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_test_result"},
#if CFG_SUPPORT_PRIV_MCR_RW
	{PRIV_CMD_ACCESS_MCR, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "set_mcr"},
	{PRIV_CMD_ACCESS_MCR, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_mcr"},
#endif
	{PRIV_CMD_SW_CTRL, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "set_sw_ctrl"},
	{PRIV_CMD_SW_CTRL, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_sw_ctrl"},

#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS
	{PRIV_CUSTOM_BWCS_CMD, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_bwcs"},
	/*                                */
	{PRIV_CUSTOM_BWCS_CMD, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_bwcs"},
#endif

	/*                                */
	{PRIV_CMD_OID, 256, 0, "set_oid"},
	/*                                */
	{PRIV_CMD_OID, 0, 256, "get_oid"},

	{PRIV_CMD_BAND_CONFIG, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "set_band"},
	{PRIV_CMD_BAND_CONFIG, 0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_band"},

	{PRIV_CMD_SET_TX_POWER, IW_PRIV_TYPE_INT | 4, 0, "set_txpower"},
	{PRIV_CMD_GET_CH_LIST, 0, IW_PRIV_TYPE_INT | 50, "get_ch_list"},
	{PRIV_CMD_DUMP_MEM, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2,
	 IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_mem"},

#if CFG_ENABLE_WIFI_DIRECT
	{PRIV_CMD_P2P_MODE, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "set_p2p_mode"},
#endif
};

static const iw_handler rIwPrivHandler[] = {
	[IOCTL_SET_INT - SIOCIWFIRSTPRIV] = priv_set_int,
	[IOCTL_GET_INT - SIOCIWFIRSTPRIV] = priv_get_int,
	[IOCTL_SET_ADDRESS - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_GET_ADDRESS - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_SET_STR - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_GET_STR - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_SET_KEY - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_GET_KEY - SIOCIWFIRSTPRIV] = NULL,
	[IOCTL_SET_STRUCT - SIOCIWFIRSTPRIV] = priv_set_struct,
	[IOCTL_GET_STRUCT - SIOCIWFIRSTPRIV] = priv_get_struct,
	[IOCTL_SET_STRUCT_FOR_EM - SIOCIWFIRSTPRIV] = priv_set_struct,
	[IOCTL_SET_INTS - SIOCIWFIRSTPRIV] = priv_set_ints,
	[IOCTL_GET_INTS - SIOCIWFIRSTPRIV] = priv_get_ints,
};

const struct iw_handler_def wext_handler_def = {
	.num_standard = 0,
	.num_private = (__u16) sizeof(rIwPrivHandler) / sizeof(iw_handler),
	.num_private_args = (__u16) sizeof(rIwPrivTable) / sizeof(struct iw_priv_args),
	.standard = (iw_handler *) NULL,
	.private = rIwPrivHandler,
	.private_args = rIwPrivTable,
	.get_wireless_stats = wext_get_wireless_stats,
};

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                                 
 
                                            
                                                     
                                                
                                                      
 
                                   
                                     
 
       
*/
/*                                                                            */
BOOLEAN
wextSrchDesiredWPAIE(IN PUINT_8 pucIEStart,
		     IN INT_32 i4TotalIeLen, IN UINT_8 ucDesiredElemId, OUT PUINT_8 * ppucDesiredIE)
{
	INT_32 i4InfoElemLen;

	ASSERT(pucIEStart);
	ASSERT(ppucDesiredIE);

	while (i4TotalIeLen >= 2) {
		i4InfoElemLen = (INT_32) pucIEStart[1] + 2;

		if (pucIEStart[0] == ucDesiredElemId && i4InfoElemLen <= i4TotalIeLen) {
			if (ucDesiredElemId != 0xDD) {
				/*               */
				*ppucDesiredIE = &pucIEStart[0];
				return TRUE;
			} else {
				/*                           */
				if (pucIEStart[1] >= 4) {
					if (memcmp(&pucIEStart[2], "\x00\x50\xf2\x01", 4) == 0) {
						*ppucDesiredIE = &pucIEStart[0];
						return TRUE;
					}
				}	/*                     */
			}	/*                   */
		}

		/*                   */
		/*                                  */
		i4TotalIeLen -= i4InfoElemLen;
		pucIEStart += i4InfoElemLen;
	}

	return FALSE;
}				/*                         */


#if CFG_SUPPORT_WAPI
/*                                                                            */
/* 
                                                    
 
                                            
                                                     
                                                      
 
                                   
                                     
 
       
*/
/*                                                                            */
BOOLEAN
wextSrchDesiredWAPIIE(IN PUINT_8 pucIEStart, IN INT_32 i4TotalIeLen, OUT PUINT_8 * ppucDesiredIE)
{
	INT_32 i4InfoElemLen;

	ASSERT(pucIEStart);
	ASSERT(ppucDesiredIE);

	while (i4TotalIeLen >= 2) {
		i4InfoElemLen = (INT_32) pucIEStart[1] + 2;

		if (pucIEStart[0] == ELEM_ID_WAPI && i4InfoElemLen <= i4TotalIeLen) {
			*ppucDesiredIE = &pucIEStart[0];
			return TRUE;
		}

		/*                   */
		/*                                  */
		i4TotalIeLen -= i4InfoElemLen;
		pucIEStart += i4InfoElemLen;
	}

	return FALSE;
}				/*                       */
#endif


#if CFG_SUPPORT_WPS
/*                                                                            */
/* 
                                                                             
 
                                            
                                                     
                                                
                                                      
 
                                   
                                     
 
       
*/
/*                                                                            */
BOOLEAN
wextSrchDesiredWPSIE(IN PUINT_8 pucIEStart,
		     IN INT_32 i4TotalIeLen, IN UINT_8 ucDesiredElemId, OUT PUINT_8 * ppucDesiredIE)
{
	INT_32 i4InfoElemLen;

	ASSERT(pucIEStart);
	ASSERT(ppucDesiredIE);

	while (i4TotalIeLen >= 2) {
		i4InfoElemLen = (INT_32) pucIEStart[1] + 2;

		if (pucIEStart[0] == ucDesiredElemId && i4InfoElemLen <= i4TotalIeLen) {
			if (ucDesiredElemId != 0xDD) {
				/*               */
				*ppucDesiredIE = &pucIEStart[0];
				return TRUE;
			} else {
				/*                           */
				if (pucIEStart[1] >= 4) {
					if (memcmp(&pucIEStart[2], "\x00\x50\xf2\x04", 4) == 0) {
						*ppucDesiredIE = &pucIEStart[0];
						return TRUE;
					}
				}	/*                     */
			}	/*                   */
		}

		/*                   */
		/*                                  */
		i4TotalIeLen -= i4InfoElemLen;
		pucIEStart += i4InfoElemLen;
	}

	return FALSE;
}				/*                         */
#endif


/*                                                                            */
/* 
                                                      
 
                                         
                             
                                                         
                           
 
                        
 
                                                       
                                              
*/
/*                                                                            */
static int
wext_get_name(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, OUT char *pcName, IN char *pcExtra)
{
	ENUM_PARAM_NETWORK_TYPE_T eNetWorkType;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(pcName);
	if (FALSE == GLUE_CHK_PR2(prNetDev, pcName)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	if (netif_carrier_ok(prNetDev)) {

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidQueryNetworkTypeInUse,
				   &eNetWorkType,
				   sizeof(eNetWorkType), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

		switch (eNetWorkType) {
		case PARAM_NETWORK_TYPE_DS:
			strcpy(pcName, "IEEE 802.11b");
			break;
		case PARAM_NETWORK_TYPE_OFDM24:
			strcpy(pcName, "IEEE 802.11bgn");
			break;
		case PARAM_NETWORK_TYPE_AUTOMODE:
		case PARAM_NETWORK_TYPE_OFDM5:
			strcpy(pcName, "IEEE 802.11abgn");
			break;
		case PARAM_NETWORK_TYPE_FH:
		default:
			strcpy(pcName, "IEEE 802.11");
			break;
		}
	} else {
		strcpy(pcName, "Disconnected");
	}

	return 0;
}				/*               */

/*                                                                            */
/* 
                                                             
 
                                        
                           
                                                         
                         
 
                        
                                                                      
                                            
 
                                                                               
                                                                          
*/
/*                                                                            */
static int
wext_set_freq(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo, IN struct iw_freq *prIwFreq, IN char *pcExtra)
{

#if 0
	UINT_32 u4ChnlFreq;	/*                                        */

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prIwFreq);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prIwFreq)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*
                                              
                                                            
  */

	/*                                               */
	if ((prIwFreq->e == 1) && (prIwFreq->m >= (int)2.412e8) && (prIwFreq->m <= (int)2.484e8)) {

		/*                      */
		u4ChnlFreq = (UINT_32) (prIwFreq->m / (KILO / 10));

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetFrequency,
				   &u4ChnlFreq, sizeof(u4ChnlFreq), FALSE, FALSE, FALSE, &u4BufLen);

		if (WLAN_STATUS_SUCCESS != rStatus) {
			return -EINVAL;
		}
	}
	/*                           */
	else if ((prIwFreq->m > KILO) || (prIwFreq->e > 0)) {
		return -EOPNOTSUPP;
	} else {
		/*                                 */
		u4ChnlFreq = (UINT_32) prIwFreq->m;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetChannel,
				   &u4ChnlFreq, sizeof(u4ChnlFreq), FALSE, FALSE, FALSE, &u4BufLen);




		if (WLAN_STATUS_SUCCESS != rStatus) {
			return -EINVAL;
		}
	}

#endif

	return 0;

}				/*               */


/*                                                                            */
/* 
                                                             
 
                                        
                            
                                                           
                          
 
                                
                             
 
                                                                              
*/
/*                                                                            */
static int
wext_get_freq(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, OUT struct iw_freq *prIwFreq, IN char *pcExtra)
{
	UINT_32 u4Channel = 0;


	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prIwFreq);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prIwFreq)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                            */
	if (!netif_carrier_ok(prNetDev)) {
		return -ENOTCONN;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryFrequency,
			   &u4Channel, sizeof(u4Channel), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	prIwFreq->m = (int)u4Channel;	/*             */
	prIwFreq->e = 3;

	return 0;

}				/*               */


/*                                                                            */
/* 
                               
 
                                        
                            
                                                   
                          
 
                        
                                                   
 
                                                             
*/
/*                                                                            */
static int
wext_set_mode(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo, IN unsigned int *pu4Mode, IN char *pcExtra)
{
	ENUM_PARAM_OP_MODE_T eOpMode;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(pu4Mode);
	if (FALSE == GLUE_CHK_PR2(prNetDev, pu4Mode)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	switch (*pu4Mode) {
	case IW_MODE_AUTO:
		eOpMode = NET_TYPE_AUTO_SWITCH;
		break;

	case IW_MODE_ADHOC:
		eOpMode = NET_TYPE_IBSS;
		break;

	case IW_MODE_INFRA:
		eOpMode = NET_TYPE_INFRA;
		break;

	default:
		DBGLOG(INIT, INFO, ("%s(): Set UNSUPPORTED Mode = %d.\n", __func__, *pu4Mode));
		return -EOPNOTSUPP;
	}

	/*                                                          */

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetInfrastructureMode,
			   &eOpMode, sizeof(eOpMode), FALSE, FALSE, TRUE, FALSE, &u4BufLen);


	/*                                                 */

	/*                */
	prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;
	prGlueInfo->rWpaInfo.u4KeyMgmt = 0;
	prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
#if CFG_SUPPORT_802_11W
	prGlueInfo->rWpaInfo.u4Mfp = IW_AUTH_MFP_DISABLED;
#endif

	return 0;
}				/*               */

/*                                                                            */
/* 
                               
 
                                           
                              
                                                                 
                          
 
                             
                            
 
                                                                             
*/
/*                                                                            */
static int
wext_get_mode(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo, OUT unsigned int *pu4Mode, IN char *pcExtra)
{
	ENUM_PARAM_OP_MODE_T eOpMode;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(pu4Mode);
	if (FALSE == GLUE_CHK_PR2(prNetDev, pu4Mode)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryInfrastructureMode,
			   &eOpMode, sizeof(eOpMode), TRUE, FALSE, FALSE, FALSE, &u4BufLen);



	switch (eOpMode) {
	case NET_TYPE_IBSS:
		*pu4Mode = IW_MODE_ADHOC;
		break;

	case NET_TYPE_INFRA:
		*pu4Mode = IW_MODE_INFRA;
		break;

	case NET_TYPE_AUTO_SWITCH:
		*pu4Mode = IW_MODE_AUTO;
		break;

	default:
		DBGLOG(INIT, INFO, ("%s(): Get UNKNOWN Mode.\n", __func__));
		return -EINVAL;
	}

	return 0;
}				/*               */

/*                                                                            */
/* 
                                                                        
 
                                        
                            
                                                            
                                                                            
                                                                                
                                 
 
                             
 
                                                                          
*/
/*                                                                            */
static int
wext_get_range(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, IN struct iw_point *prData, OUT char *pcExtra)
{
	struct iw_range *prRange = NULL;
	PARAM_RATES_EX aucSuppRate = { 0 };	/*              */
	int i = 0;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR2(prNetDev, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	prRange = (struct iw_range *)pcExtra;

	memset(prRange, 0, sizeof(*prRange));
	prRange->throughput = 20000000;	/*        */
	prRange->min_nwid = 0;	/*          */
	prRange->max_nwid = 0;	/*          */

	/*                           */

	/*                                             */
	prRange->event_capa[0] = (IW_EVENT_CAPA_K_0 | IW_EVENT_CAPA_MASK(SIOCGIWAP)
				  | IW_EVENT_CAPA_MASK(SIOCGIWSCAN)
				  /*                                          
                                           
                                                    
                                             
                                            
       */
	    );
	prRange->event_capa[1] = IW_EVENT_CAPA_K_1;

	/*                                        */
	prRange->num_channels = (__u16) NUM_CHANNELS;
	prRange->num_frequency = (__u8) NUM_CHANNELS;
	for (i = 0; i < NUM_CHANNELS; i++) {
		/*                                           */
		prRange->freq[i].i = i + 1;
		prRange->freq[i].m = channel_freq[i];
		prRange->freq[i].e = 6;	/*                        */
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQuerySupportedRates,
			   &aucSuppRate, sizeof(aucSuppRate), TRUE, FALSE, FALSE, FALSE, &u4BufLen);



	for (i = 0; i < IW_MAX_BITRATES && i < PARAM_MAX_LEN_RATES_EX; i++) {
		if (aucSuppRate[i] == 0) {
			break;
		}
		prRange->bitrate[i] = (aucSuppRate[i] & 0x7F) * 500000;	/*         */
	}
	prRange->num_bitrates = i;

	prRange->min_rts = 0;
	prRange->max_rts = 2347;
	prRange->min_frag = 256;
	prRange->max_frag = 2346;

	prRange->min_pmp = 0;	/*                            */
	prRange->max_pmp = 0;	/*                            */
	prRange->min_pmt = 0;	/*                            */
	prRange->max_pmt = 0;	/*                            */
	prRange->pmp_flags = IW_POWER_RELATIVE;	/*                 */
	prRange->pmt_flags = IW_POWER_ON;	/*                 */
	prRange->pm_capa = IW_POWER_ON;	/*                            */

	prRange->encoding_size[0] = 5;	/*       */
	prRange->encoding_size[1] = 16;	/*      */
	prRange->encoding_size[2] = 16;	/*      */
	prRange->encoding_size[3] = 16;	/*      */
	prRange->encoding_size[4] = 13;	/*        */
	prRange->encoding_size[5] = 16;	/*        */
	prRange->num_encoding_sizes = 6;
	prRange->max_encoding_tokens = 6;	/*        */

#if WIRELESS_EXT < 17
	prRange->txpower_capa = 0x0002;	/*                   */
#else
	prRange->txpower_capa = IW_TXPOW_RELATIVE;
#endif
	prRange->num_txpower = 5;
	prRange->txpower[0] = 0;	/*         */
	prRange->txpower[1] = 25;	/*     */
	prRange->txpower[2] = 50;	/*     */
	prRange->txpower[3] = 100;	/*      */

	prRange->we_version_compiled = WIRELESS_EXT;
	prRange->we_version_source = WIRELESS_EXT;

	prRange->retry_capa = IW_RETRY_LIMIT;
	prRange->retry_flags = IW_RETRY_LIMIT;
	prRange->min_retry = 7;
	prRange->max_retry = 7;
	prRange->r_time_flags = IW_RETRY_ON;
	prRange->min_r_time = 0;
	prRange->max_r_time = 0;

	/*                                  */
	/*                                                                   */
	prRange->sensitivity = -83;	/*             */
	prRange->max_qual.qual = 100;	/*          */
	prRange->max_qual.level = (__u8) (0x100 - 0);	/*           */
	prRange->max_qual.noise = (__u8) (0x100 - 0);	/*           */

	/*          */
#if WIRELESS_EXT > 17
	prRange->enc_capa = IW_ENC_CAPA_WPA |
	    IW_ENC_CAPA_WPA2 | IW_ENC_CAPA_CIPHER_TKIP | IW_ENC_CAPA_CIPHER_CCMP;
#endif

	/*                            */
	/*                            */
	/*                                            */

	/*                                  */
	/*                                           */

	return 0;
}				/*                */


/*                                                                            */
/* 
                                       
 
                                        
                            
                                                                               
                          
 
                        
 
                                            
*/
/*                                                                            */
static int
wext_set_ap(IN struct net_device *prDev,
	    IN struct iw_request_info *prIwrInfo, IN struct sockaddr *prAddr, IN char *pcExtra)
{
	return 0;
}				/*             */


/*                                                                            */
/* 
                               
 
                                        
                            
                                                                             
                          
 
                                
                              
 
                                                                          
*/
/*                                                                            */
static int
wext_get_ap(IN struct net_device *prNetDev,
	    IN struct iw_request_info *prIwrInfo, OUT struct sockaddr *prAddr, IN char *pcExtra)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prAddr);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prAddr)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                    */
	/*                   */
	/*   */

	if (prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_DISCONNECTED) {
		memset(prAddr, 0, 6);
		return 0;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryBssid,
			   prAddr->sa_data, ETH_ALEN, TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	return 0;
}				/*             */

/*                                                                            */
/* 
                                       
 
                                        
                            
                                               
                                                                           
 
                        
                                                   
                                                 
 
                                                  
*/
/*                                                                            */
static int
wext_set_mlme(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, IN struct iw_point *prData, IN char *pcExtra)
{
	struct iw_mlme *prMlme = NULL;


	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR2(prNetDev, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	prMlme = (struct iw_mlme *)pcExtra;
	if (prMlme->cmd == IW_MLME_DEAUTH || prMlme->cmd == IW_MLME_DISASSOC) {
		if (!netif_carrier_ok(prNetDev)) {
			DBGLOG(INIT, INFO,
			       ("[wifi] Set MLME Deauth/Disassoc, but netif_carrier_off\n"));
			return 0;
		}

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetDisassociate,
				   NULL, 0, FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		return 0;
	} else {
		DBGLOG(INIT, INFO, ("[wifi] unsupported IW_MLME_ command :%d\n", prMlme->cmd));
		return -EOPNOTSUPP;
	}
}				/*               */

/*                                                                            */
/* 
                               
 
                                        
                            
                         
                          
 
                        
                                  
 
                                   
*/
/*                                                                            */
static int
wext_set_scan(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, IN union iwreq_data *prData, IN char *pcExtra)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	int essid_len = 0;

	ASSERT(prNetDev);
	if (FALSE == GLUE_CHK_DEV(prNetDev)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

#if WIRELESS_EXT > 17
	/*               */
	if (prData) {
		essid_len =
		    ((struct iw_scan_req *)(((struct iw_point *)prData)->pointer))->essid_len;
	}
#endif

	init_completion(&prGlueInfo->rScanComp);

	/*                                                       */

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetBssidListScan,
			   pcExtra, essid_len, FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	/*                                                                                */
	/*                                                                               */


	return 0;
}				/*               */


/*                                                                            */
/* 
                                  
 
*/
/*                                                                            */
static inline int snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	size_t i;
	char *pos = buf, *end = buf + buf_size;
	int ret;

	if (buf_size == 0)
		return 0;

	for (i = 0; i < len; i++) {
		ret = snprintf(pos, end - pos, "%02x", data[i]);
		if (ret < 0 || ret >= end - pos) {
			end[-1] = '\0';
			return pos - buf;
		}
		pos += ret;
	}
	end[-1] = '\0';
	return pos - buf;
}


/*                                                                            */
/* 
                                                                             
 
                                        
                            
                                                                                
                                                                             
                        
                                                                            
                                                                                
                                 
 
                        
                                                    
                                
 
                                                                           
                      
*/
/*                                                                            */
static int
wext_get_scan(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo,
	      IN OUT struct iw_point *prData, IN char *pcExtra)
{
	UINT_32 i = 0;
	UINT_32 j = 0;
	P_PARAM_BSSID_LIST_EX_T prList = NULL;
	P_PARAM_BSSID_EX_T prBss = NULL;
	P_PARAM_VARIABLE_IE_T prDesiredIE = NULL;
	struct iw_event iwEvent;	/*                       */

	/*                               */
	char *pcCur = NULL;
	/*                                                        */
	char *pcValidEntryEnd = NULL;
	char *pcEnd = NULL;	/*                     */

	UINT_32 u4AllocBufLen = 0;

	/*                          */
	UINT_32 u4HighestRate = 0;
	char aucRatesBuf[64];
	UINT_32 u4BufIndex;

	/*              */
	int ret = 0;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prData);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prData, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                            */
	pcCur = pcExtra;
	pcValidEntryEnd = pcExtra;
	pcEnd = pcExtra + prData->length;	/*                     */

	/*                                                                  */
	u4AllocBufLen = prData->length;
	prList = kalMemAlloc(u4AllocBufLen, VIR_MEM_TYPE);
	if (prList == NULL) {
		DBGLOG(INIT, INFO, ("[wifi] no memory for scan list:%d\n", prData->length));
		ret = -ENOMEM;
		goto error;
	}
	prList->u4NumberOfItems = 0;

	/*                */
	/*                                     */
	/*                                                                                */

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryBssidList,
			   prList, u4AllocBufLen, TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus == WLAN_STATUS_INVALID_LENGTH) {
		/*                                    */
		/*                                                                          */

#if WIRELESS_EXT >= 17
		/*                                                                
                                                                       
   */
		ret = -E2BIG;
		/*                                                           */
		prData->length = (__u16) u4BufLen;
		goto error;
#else
		/*                                                                      
                                   
   */
		kalMemFree(prList, VIR_MEM_TYPE, u4AllocBufLen);

		u4AllocBufLen = u4BufLen;
		prList = kalMemAlloc(u4AllocBufLen, VIR_MEM_TYPE);
		if (prList == NULL) {
			DBGLOG(INIT, INFO,
			       ("[wifi] no memory for larger scan list :%ld\n", u4BufLen));
			ret = -ENOMEM;
			goto error;
		}
		prList->NumberOfItems = 0;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidQueryBssidList,
				   prList, u4AllocBufLen, TRUE, FALSE, FALSE, FALSE, &u4BufLen);

		if (rStatus == WLAN_STATUS_INVALID_LENGTH) {
			DBGLOG(INIT, INFO,
			       ("[wifi] larger buf:%d result:%ld\n", u4AllocBufLen, u4BufLen));
			ret = -E2BIG;
			prData->length = (__u16) u4BufLen;
			goto error;
		}
#endif				/*                    */

	}


	if (prList->u4NumberOfItems > CFG_MAX_NUM_BSS_LIST) {
		DBGLOG(INIT, INFO, ("[wifi] strange scan result count:%ld\n",
				    prList->u4NumberOfItems));
		goto error;
	}

	/*                                          */
	prBss = &prList->arBssid[0];	/*                        */
	for (i = 0; i < prList->u4NumberOfItems; ++i) {
		/*       */
		iwEvent.cmd = SIOCGIWAP;
		iwEvent.len = IW_EV_ADDR_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(iwEvent.u.ap_addr.sa_data, prBss->arMacAddress, ETH_ALEN);
		memcpy(pcCur, &iwEvent, IW_EV_ADDR_LEN);
		pcCur += IW_EV_ADDR_LEN;

		/*      */
		iwEvent.cmd = SIOCGIWESSID;
		/*                                                                  */
		iwEvent.u.essid.length = (__u16) prBss->rSsid.u4SsidLen;
		iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.essid.length;

		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.essid.flags = 1;
		iwEvent.u.essid.pointer = NULL;

#if WIRELESS_EXT <= 18
		memcpy(pcCur, &iwEvent, iwEvent.len);
#else
		memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
		memcpy(pcCur + IW_EV_LCP_LEN,
		       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
		memcpy(pcCur + IW_EV_POINT_LEN, prBss->rSsid.aucSsid, iwEvent.u.essid.length);
		pcCur += iwEvent.len;
		/*           */
		iwEvent.cmd = SIOCGIWFREQ;
		iwEvent.len = IW_EV_FREQ_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.freq.m = prBss->rConfiguration.u4DSConfig;
		iwEvent.u.freq.e = 3;	/*          */
		iwEvent.u.freq.i = 0;
		memcpy(pcCur, &iwEvent, IW_EV_FREQ_LEN);
		pcCur += IW_EV_FREQ_LEN;

		/*                */
		iwEvent.cmd = SIOCGIWMODE;
		iwEvent.len = IW_EV_UINT_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		if (prBss->eOpMode == NET_TYPE_IBSS) {
			iwEvent.u.mode = IW_MODE_ADHOC;
		} else if (prBss->eOpMode == NET_TYPE_INFRA) {
			iwEvent.u.mode = IW_MODE_INFRA;
		} else {
			iwEvent.u.mode = IW_MODE_AUTO;
		}
		memcpy(pcCur, &iwEvent, IW_EV_UINT_LEN);
		pcCur += IW_EV_UINT_LEN;

		/*         */
		iwEvent.cmd = IWEVQUAL;
		iwEvent.len = IW_EV_QUAL_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.qual.qual = 0;	/*                           */
		/*                                               */
		iwEvent.u.qual.level = 0x100 + prBss->rRssi;
		iwEvent.u.qual.noise = 0;	/*                         */
		iwEvent.u.qual.updated = IW_QUAL_QUAL_INVALID | IW_QUAL_LEVEL_UPDATED
		    | IW_QUAL_NOISE_INVALID;
		memcpy(pcCur, &iwEvent, IW_EV_QUAL_LEN);
		pcCur += IW_EV_QUAL_LEN;

		/*               */
		iwEvent.cmd = SIOCGIWENCODE;
		iwEvent.len = IW_EV_POINT_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.data.pointer = NULL;
		iwEvent.u.data.flags = 0;
		iwEvent.u.data.length = 0;
		if (!prBss->u4Privacy) {
			iwEvent.u.data.flags |= IW_ENCODE_DISABLED;
		}
#if WIRELESS_EXT <= 18
		memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
		memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
		memcpy(pcCur + IW_EV_LCP_LEN,
		       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
		pcCur += IW_EV_POINT_LEN;

		/*                            */
		u4BufIndex = sprintf(aucRatesBuf, "Rates (Mb/s):");
		u4HighestRate = 0;
		for (j = 0; j < PARAM_MAX_LEN_RATES_EX; ++j) {
			UINT_8 curRate = prBss->rSupportedRates[j] & 0x7F;
			if (curRate == 0) {
				break;
			}

			if (curRate > u4HighestRate) {
				u4HighestRate = curRate;
			}

			if (curRate == RATE_5_5M) {
				u4BufIndex += sprintf(aucRatesBuf + u4BufIndex, " 5.5");
			} else {
				u4BufIndex += sprintf(aucRatesBuf + u4BufIndex, " %d", curRate / 2);
			}
#if DBG
			if (u4BufIndex > sizeof(aucRatesBuf)) {
				/*                                 */
				break;
			}
#endif
		}
		/*                      */
		iwEvent.cmd = SIOCGIWRATE;
		iwEvent.len = IW_EV_PARAM_LEN;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.bitrate.value = u4HighestRate * 500000;
		iwEvent.u.bitrate.fixed = 0;
		iwEvent.u.bitrate.disabled = 0;
		iwEvent.u.bitrate.flags = 0;
		memcpy(pcCur, &iwEvent, iwEvent.len);
		pcCur += iwEvent.len;

#if WIRELESS_EXT >= 15		/*                                           */
		/*                       */
		iwEvent.cmd = IWEVCUSTOM;
		iwEvent.u.data.length = u4BufIndex;
		iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
		if ((pcCur + iwEvent.len) > pcEnd)
			break;
		iwEvent.u.data.flags = 0;
#if WIRELESS_EXT <= 18
		memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
		memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
		memcpy(pcCur + IW_EV_LCP_LEN,
		       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
		memcpy(pcCur + IW_EV_POINT_LEN, aucRatesBuf, u4BufIndex);
		pcCur += iwEvent.len;
#endif				/*                    */


		if (wextSrchDesiredWPAIE(&prBss->aucIEs[sizeof(PARAM_FIXED_IEs)],
					 prBss->u4IELength - sizeof(PARAM_FIXED_IEs),
					 0xDD, (PUINT_8 *) &prDesiredIE)) {
			iwEvent.cmd = IWEVGENIE;
			iwEvent.u.data.flags = 1;
			iwEvent.u.data.length = 2 + (__u16) prDesiredIE->ucLength;
			iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
			if ((pcCur + iwEvent.len) > pcEnd)
				break;
#if WIRELESS_EXT <= 18
			memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
			memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
			memcpy(pcCur + IW_EV_LCP_LEN,
			       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
			memcpy(pcCur + IW_EV_POINT_LEN, prDesiredIE, 2 + prDesiredIE->ucLength);
			pcCur += iwEvent.len;
		}
#if CFG_SUPPORT_WPS		/*                                             */
		if (wextSrchDesiredWPSIE(&prBss->aucIEs[sizeof(PARAM_FIXED_IEs)],
					 prBss->u4IELength - sizeof(PARAM_FIXED_IEs),
					 0xDD, (PUINT_8 *) &prDesiredIE)) {
			iwEvent.cmd = IWEVGENIE;
			iwEvent.u.data.flags = 1;
			iwEvent.u.data.length = 2 + (__u16) prDesiredIE->ucLength;
			iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
			if ((pcCur + iwEvent.len) > pcEnd)
				break;
#if WIRELESS_EXT <= 18
			memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
			memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
			memcpy(pcCur + IW_EV_LCP_LEN,
			       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
			memcpy(pcCur + IW_EV_POINT_LEN, prDesiredIE, 2 + prDesiredIE->ucLength);
			pcCur += iwEvent.len;
		}
#endif


		/*                                                            */
		/*                                 */
		if (wextSrchDesiredWPAIE(&prBss->aucIEs[sizeof(PARAM_FIXED_IEs)],
					 prBss->u4IELength - sizeof(PARAM_FIXED_IEs),
					 0x30, (PUINT_8 *) &prDesiredIE)) {

			iwEvent.cmd = IWEVGENIE;
			iwEvent.u.data.flags = 1;
			iwEvent.u.data.length = 2 + (__u16) prDesiredIE->ucLength;
			iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
			if ((pcCur + iwEvent.len) > pcEnd)
				break;
#if WIRELESS_EXT <= 18
			memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
			memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
			memcpy(pcCur + IW_EV_LCP_LEN,
			       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
			memcpy(pcCur + IW_EV_POINT_LEN, prDesiredIE, 2 + prDesiredIE->ucLength);
			pcCur += iwEvent.len;
		}
#if CFG_SUPPORT_WAPI		/*          */
		if (wextSrchDesiredWAPIIE(&prBss->aucIEs[sizeof(PARAM_FIXED_IEs)],
					  prBss->u4IELength - sizeof(PARAM_FIXED_IEs),
					  (PUINT_8 *) &prDesiredIE)) {

#if 0
			iwEvent.cmd = IWEVGENIE;
			iwEvent.u.data.flags = 1;
			iwEvent.u.data.length = 2 + (__u16) prDesiredIE->ucLength;
			iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
			if ((pcCur + iwEvent.len) > pcEnd)
				break;
#if WIRELESS_EXT <= 18
			memcpy(pcCur, &iwEvent, IW_EV_POINT_LEN);
#else
			memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
			memcpy(pcCur + IW_EV_LCP_LEN,
			       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);
#endif
			memcpy(pcCur + IW_EV_POINT_LEN, prDesiredIE, 2 + prDesiredIE->ucLength);
			pcCur += iwEvent.len;
#else
			iwEvent.cmd = IWEVCUSTOM;
			iwEvent.u.data.length = (2 + prDesiredIE->ucLength) * 2 + 8 /*          */;
			iwEvent.len = IW_EV_POINT_LEN + iwEvent.u.data.length;
			if ((pcCur + iwEvent.len) > pcEnd)
				break;
			iwEvent.u.data.flags = 1;

			memcpy(pcCur, &iwEvent, IW_EV_LCP_LEN);
			memcpy(pcCur + IW_EV_LCP_LEN,
			       &iwEvent.u.data.length, sizeof(struct iw_point) - IW_EV_POINT_OFF);

			pcCur += (IW_EV_POINT_LEN);

			pcCur += sprintf(pcCur, "wapi_ie=");

			snprintf_hex(pcCur, pcEnd - pcCur, (UINT_8 *) prDesiredIE,
				     prDesiredIE->ucLength + 2);

			pcCur += (2 + prDesiredIE->ucLength) * 2 /*             */;
#endif
		}
#endif
		/*                                              */
		pcValidEntryEnd = pcCur;
		/*                  */
		prBss = (P_PARAM_BSSID_EX_T) ((char *)prBss + prBss->u4Length);
	}

	/*                                                             
                 
  */
	prData->length = (pcValidEntryEnd - pcExtra);
	/*                                                                          */

	/*                                                                               */

 error :
	/*                         */
	if (prList) {
		kalMemFree(prList, VIR_MEM_TYPE, u4AllocBufLen);
	}

	return ret;
}				/*               */

/*                                                                            */
/* 
                                           
 
                                        
                            
                                                
                                                             
 
                                
                                                
                                          
                                               
 
                                                                             
*/
/*                                                                            */
static int
wext_set_essid(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, IN struct iw_point *prEssid, IN char *pcExtra)
{
	PARAM_SSID_T rNewSsid;
	UINT_32 cipher;
	ENUM_PARAM_ENCRYPTION_STATUS_T eEncStatus;
	ENUM_PARAM_AUTH_MODE_T eAuthMode;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prEssid);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prEssid, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	if (prEssid->length > IW_ESSID_MAX_SIZE) {
		return -E2BIG;
	}


	/*               */
	if (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_DISABLED) {
		eAuthMode = (prGlueInfo->rWpaInfo.u4AuthAlg == IW_AUTH_ALG_OPEN_SYSTEM) ?
		    AUTH_MODE_OPEN : AUTH_MODE_AUTO_SWITCH;
		/*                                                                      */
		/*                                                     */
	} else {
		/*               */
		switch (prGlueInfo->rWpaInfo.u4KeyMgmt) {
		case IW_AUTH_KEY_MGMT_802_1X:
			eAuthMode =
			    (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_WPA) ?
			    AUTH_MODE_WPA : AUTH_MODE_WPA2;
			/*                                                      */
			/*                                           */
			break;
		case IW_AUTH_KEY_MGMT_PSK:
			eAuthMode =
			    (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_WPA) ?
			    AUTH_MODE_WPA_PSK : AUTH_MODE_WPA2_PSK;
			/*                                                      */
			/*                                               */
			break;
#if CFG_SUPPORT_WAPI		/*          */
		case IW_AUTH_KEY_MGMT_WAPI_PSK:
			break;
		case IW_AUTH_KEY_MGMT_WAPI_CERT:
			break;
#endif

/*                                         */
/*                                 */
/*                                 */
/*                                                              */
/*        */
/*        */
#if CFG_SUPPORT_802_11W
		case IW_AUTH_KEY_MGMT_802_1X_SHA256:
			eAuthMode = AUTH_MODE_WPA2;
			break;
		case IW_AUTH_KEY_MGMT_PSK_SHA256:
			eAuthMode = AUTH_MODE_WPA2_PSK;
			break;
#endif
		default:
			/*                                                                              */
			/*                                  */
			eAuthMode = AUTH_MODE_AUTO_SWITCH;
			break;
		}
	}


	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetAuthMode,
			   &eAuthMode, sizeof(eAuthMode), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

	/*                       */
	cipher = prGlueInfo->rWpaInfo.u4CipherGroup | prGlueInfo->rWpaInfo.u4CipherPairwise;
	if (cipher & IW_AUTH_CIPHER_CCMP) {
		/*                                                            */
		eEncStatus = ENUM_ENCRYPTION3_ENABLED;
	} else if (cipher & IW_AUTH_CIPHER_TKIP) {
		/*                                                            */
		eEncStatus = ENUM_ENCRYPTION2_ENABLED;
	} else if (cipher & (IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40)) {
		/*                                                            */
		eEncStatus = ENUM_ENCRYPTION1_ENABLED;
	} else if (cipher & IW_AUTH_CIPHER_NONE) {
		/*                                                            */
		if (prGlueInfo->rWpaInfo.fgPrivacyInvoke)
			eEncStatus = ENUM_ENCRYPTION1_ENABLED;
		else
			eEncStatus = ENUM_ENCRYPTION_DISABLED;
	} else {
		/*                                                               */
		eEncStatus = ENUM_ENCRYPTION_DISABLED;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetEncryptionStatus,
			   &eEncStatus, sizeof(eEncStatus), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

#if WIRELESS_EXT < 21
	/*                                                                       
                                   
  */
	rNewSsid.u4SsidLen = (prEssid->length) ? prEssid->length - 1 : 0;
#else
	rNewSsid.u4SsidLen = prEssid->length;
#endif
	kalMemCopy(rNewSsid.aucSsid, pcExtra, rNewSsid.u4SsidLen);

	/*
                                                
                                                                        
  */

	if (kalIoctl(prGlueInfo,
		     wlanoidSetSsid,
		     (PVOID) & rNewSsid,
		     sizeof(PARAM_SSID_T),
		     FALSE, FALSE, TRUE, FALSE, &u4BufLen) != WLAN_STATUS_SUCCESS) {
		/*                                            */
		return -EFAULT;
	}


	return 0;
}				/*                */

/*                                                                            */
/* 
                                           
 
                                        
                            
                                                                                
                                                              
 
                                
                              
 
                                                                
*/
/*                                                                            */
/*                           */
static int
wext_get_essid(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, IN struct iw_point *prEssid, OUT char *pcExtra)
{
	/*                    */

	P_PARAM_SSID_T prSsid;
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prEssid);
	ASSERT(pcExtra);

	if (FALSE == GLUE_CHK_PR3(prNetDev, prEssid, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                    */
	/*                   */
	/*   */

	prSsid = kalMemAlloc(sizeof(PARAM_SSID_T), VIR_MEM_TYPE);

	if (!prSsid) {
		return -ENOMEM;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQuerySsid,
			   prSsid, sizeof(PARAM_SSID_T), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	if ((rStatus == WLAN_STATUS_SUCCESS) && (prSsid->u4SsidLen <= MAX_SSID_LEN)) {
		kalMemCopy(pcExtra, prSsid->aucSsid, prSsid->u4SsidLen);
		prEssid->length = prSsid->u4SsidLen;
		prEssid->flags = 1;
	}

	kalMemFree(prSsid, VIR_MEM_TYPE, sizeof(PARAM_SSID_T));

	return 0;
}				/*                */


#if 0

/*                                                                            */
/* 
                                                     
                                                                   
                                                                                 
                                                                              
 
                                                              
                                                        
                                                          
                                                        
 
                                        
                                   
*/
/*                                                                            */
int
wext_set_rate(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo, IN struct iw_param *prRate, IN char *pcExtra)
{
	PARAM_RATES_EX aucSuppRate = { 0 };
	PARAM_RATES_EX aucNewRate = { 0 };
	UINT_32 u4NewRateLen = 0;
	UINT_32 i;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prRate);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prRate)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*
                                                                
                                                                   
  */

	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQuerySupportedRates,
				       &aucSuppRate, sizeof(aucSuppRate), &u4BufLen);

	/*            */
	if (prRate->value < 0) {
		if (prRate->fixed == 0) {
			/*                          */

			/*                                   */
			/*                                                                    */
			rStatus = wlanSetInformation(prGlueInfo->prAdapter,
						     wlanoidSetDesiredRates,
						     &aucSuppRate, sizeof(aucSuppRate), &u4BufLen);
			return 0;
		} else {
			/*                           */

			/*                              */
			return -EINVAL;
		}
	}


	aucNewRate[0] = prRate->value / 500000;	/*                 */

	for (i = 0; i < PARAM_MAX_LEN_RATES_EX; i++) {
		/*                                    */
		if (aucSuppRate[i] == 0) {
			break;
		}

		if (aucNewRate[0] == aucSuppRate[i]) {
			u4NewRateLen = 1;
			break;
		}
	}

	if (u4NewRateLen == 0) {
		/*                                  */
		/*                                                */
		return -EINVAL;
	}

	if (prRate->fixed == 0) {
		/*                                       */
		for (i = 0; i < PARAM_MAX_LEN_RATES_EX; ++i) {
			if (aucSuppRate[i] == 0) {
				break;
			}

			if (aucSuppRate[i] < aucNewRate[0]) {
				aucNewRate[u4NewRateLen++] = aucSuppRate[i];
			}
		}
	}

	rStatus = wlanSetInformation(prGlueInfo->prAdapter,
				     wlanoidSetDesiredRates,
				     &aucNewRate, sizeof(aucNewRate), &u4BufLen);
	return 0;
}				/*               */

#endif

/*                                                                            */
/* 
                                    
 
                                        
                            
                                                                            
                          
 
                                
                              
 
                                                                
*/
/*                                                                            */
static int
wext_get_rate(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, OUT struct iw_param *prRate, IN char *pcExtra)
{
	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	UINT_32 u4Rate = 0;

	ASSERT(prNetDev);
	ASSERT(prRate);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prRate)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	if (!netif_carrier_ok(prNetDev)) {
		return -ENOTCONN;
	}


	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryLinkSpeed,
			   &u4Rate, sizeof(u4Rate), TRUE, FALSE, FALSE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		return -EFAULT;
	}

	prRate->value = u4Rate * 100;	/*                             */
	prRate->fixed = 0;

	return 0;
}				/*               */


/*                                                                            */
/* 
                                 
 
                                        
                            
                                                                          
                          
 
                        
                                              
 
                                                                    
*/
/*                                                                            */
static int
wext_set_rts(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwrInfo, IN struct iw_param *prRts, IN char *pcExtra)
{
	PARAM_RTS_THRESHOLD u4RtsThresh;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prRts);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prRts)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	if (prRts->disabled == 1) {
		u4RtsThresh = 2347;
	} else if (prRts->value < 0 || prRts->value > 2347) {
		return -EINVAL;
	} else {
		u4RtsThresh = (PARAM_RTS_THRESHOLD) prRts->value;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetRtsThreshold,
			   &u4RtsThresh,
			   sizeof(u4RtsThresh), FALSE, FALSE, FALSE, FALSE, &u4BufLen);



	prRts->value = (typeof(prRts->value)) u4RtsThresh;
	prRts->disabled = (prRts->value > 2347) ? 1 : 0;
	prRts->fixed = 1;

	return 0;
}				/*              */

/*                                                                            */
/* 
                                 
 
                                        
                            
                                                                           
                          
 
                    
 
                                        
*/
/*                                                                            */
static int
wext_get_rts(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwrInfo, OUT struct iw_param *prRts, IN char *pcExtra)
{
	PARAM_RTS_THRESHOLD u4RtsThresh;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prRts);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prRts)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryRtsThreshold,
			   &u4RtsThresh, sizeof(u4RtsThresh), TRUE, FALSE, FALSE, FALSE, &u4BufLen);



	prRts->value = (typeof(prRts->value)) u4RtsThresh;
	prRts->disabled = (prRts->value > 2347 || prRts->value < 0) ? 1 : 0;
	prRts->fixed = 1;

	return 0;
}				/*              */

/*                                                                            */
/* 
                                        
 
                                        
                            
                                                                             
                          
 
                    
 
                                                                    
*/
/*                                                                            */
static int
wext_get_frag(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, OUT struct iw_param *prFrag, IN char *pcExtra)
{
	ASSERT(prFrag);

	prFrag->value = 2346;
	prFrag->fixed = 1;
	prFrag->disabled = 1;
	return 0;
}				/*               */

#if 1
/*                                                                            */
/* 
                                                      
 
                                        
                            
                                                                               
                          
 
                    
 
                                                                          
                                    
*/
/*                                                                            */

static int
wext_set_txpow(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, IN struct iw_param *prTxPow, IN char *pcExtra)
{
	int ret = 0;
	/*                                       */
	ENUM_ACPI_STATE_T ePowerState;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prTxPow);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prTxPow)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	if (prTxPow->disabled) {
		/*                */
		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetDisassociate,
				   NULL, 0, FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, INFO, ("######set disassoc failed\n"));
		} else {
			DBGLOG(INIT, INFO, ("######set assoc ok\n"));
		}

		/*                              */
		ePowerState = ACPI_STATE_D0;
		DBGLOG(INIT, INFO, ("set to acpi d3(0)\n"));
		wlanSetAcpiState(prGlueInfo->prAdapter, ePowerState);

	} else {
		ePowerState = ACPI_STATE_D0;
		DBGLOG(INIT, INFO, ("set to acpi d0\n"));
		wlanSetAcpiState(prGlueInfo->prAdapter, ePowerState);
	}

	prGlueInfo->ePowerState = ePowerState;

	return ret;
}				/*                */


#endif

/*                                                                            */
/* 
                         
 
                                        
                            
                                                                                
                          
 
                    
 
                                     
*/
/*                                                                            */
static int
wext_get_txpow(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, OUT struct iw_param *prTxPow, IN char *pcExtra)
{
	/*                                       */

	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(prNetDev);
	ASSERT(prTxPow);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prTxPow)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                                                      
                                                  
  */
	/*                                        */

	/*                                          */
#if WIRELESS_EXT < 17
	prTxPow->flags = 0x0002;	/*                   */
#else
	prTxPow->flags = IW_TXPOW_RELATIVE;
#endif
	prTxPow->value = 100;
	prTxPow->fixed = 1;
	/*                                                                         */
	prTxPow->disabled = TRUE;

	return 0;
}				/*                */


/*                                                                            */
/* 
                                          
 
                                        
                            
                                                                                  
                                                 
 
                    
 
                                                                  
*/
/*                                                                            */
static int
wext_get_encode(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwrInfo, OUT struct iw_point *prEnc, IN char *pcExtra)
{
#if 1
	/*                                    */
	ENUM_PARAM_ENCRYPTION_STATUS_T eEncMode;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prEnc);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prEnc)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));


	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryEncryptionStatus,
			   &eEncMode, sizeof(eEncMode), TRUE, FALSE, FALSE, FALSE, &u4BufLen);



	switch (eEncMode) {
	case ENUM_WEP_DISABLED:
		prEnc->flags = IW_ENCODE_DISABLED;
		break;
	case ENUM_WEP_ENABLED:
		prEnc->flags = IW_ENCODE_ENABLED;
		break;
	case ENUM_WEP_KEY_ABSENT:
		prEnc->flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		break;
	default:
		prEnc->flags = IW_ENCODE_ENABLED;
		break;
	}

	/*                                              */
	prEnc->flags |= IW_ENCODE_NOKEY;
#endif
	return 0;
}				/*                 */



/*                                                                            */
/* 
                                          
 
                                        
                            
                                                                                 
                                                  
 
                    
                                       
                                                    
                                             
 
                                               
*/
/*                                                                            */
static UINT_8 wepBuf[48];

static int
wext_set_encode(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwrInfo, IN struct iw_point *prEnc, IN char *pcExtra)
{
#if 1
	ENUM_PARAM_ENCRYPTION_STATUS_T eEncStatus;
	ENUM_PARAM_AUTH_MODE_T eAuthMode;
	/*                    */
	P_PARAM_WEP_T prWepKey = (P_PARAM_WEP_T) wepBuf;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prEnc);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prEnc, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                       */
	prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;
	prGlueInfo->rWpaInfo.u4KeyMgmt = 0;
	prGlueInfo->rWpaInfo.u4CipherPairwise = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_NONE;
	prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
#if CFG_SUPPORT_802_11W
	prGlueInfo->rWpaInfo.u4Mfp = IW_AUTH_MFP_DISABLED;
#endif

	/*                        */
	if ((prEnc->flags & IW_ENCODE_MODE) == IW_ENCODE_DISABLED) {
		eAuthMode = AUTH_MODE_OPEN;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetAuthMode,
				   &eAuthMode,
				   sizeof(eAuthMode), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

		eEncStatus = ENUM_ENCRYPTION_DISABLED;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetEncryptionStatus,
				   &eEncStatus,
				   sizeof(eEncStatus), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

		return 0;
	}

	/*                               */
	/*                            */
	/*                                   */
	/*                                                   */
	/*                        */
	if (prEnc->length == 5 || prEnc->length == 13 || prEnc->length == 16) {
		/*                                 */
		prWepKey->u4KeyIndex = (prEnc->flags & IW_ENCODE_INDEX) ?
		    (prEnc->flags & IW_ENCODE_INDEX) - 1 : 0;
		if (prWepKey->u4KeyIndex > 3) {
			/*                        */
			return -EINVAL;
		}
		prWepKey->u4KeyIndex |= 0x80000000;
		prWepKey->u4Length = 12 + prEnc->length;
		prWepKey->u4KeyLength = prEnc->length;
		kalMemCopy(prWepKey->aucKeyMaterial, pcExtra, prEnc->length);


		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetAddWep,
				   prWepKey,
				   prWepKey->u4Length, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, INFO, ("wlanoidSetAddWep fail 0x%lx\n", rStatus));
			return -EFAULT;
		}

		/*                       */
		prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_SHARED_KEY | IW_AUTH_ALG_OPEN_SYSTEM;
		eAuthMode = AUTH_MODE_AUTO_SWITCH;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetAuthMode,
				   &eAuthMode,
				   sizeof(eAuthMode), FALSE, FALSE, FALSE, FALSE, &u4BufLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			/*                                                                       */
			return -EFAULT;
		}

		prGlueInfo->rWpaInfo.u4CipherPairwise =
		    IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40;
		prGlueInfo->rWpaInfo.u4CipherGroup = IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40;

		eEncStatus = ENUM_WEP_ENABLED;


		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetEncryptionStatus,
				   &eEncStatus,
				   sizeof(ENUM_PARAM_ENCRYPTION_STATUS_T),
				   FALSE, FALSE, FALSE, FALSE, &u4BufLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			/*                                                                               */
			return -EFAULT;
		}

		return 0;
	}
#endif
	return -EOPNOTSUPP;
}				/*                 */


/*                                                                            */
/* 
                                 
 
                                        
                            
                                                                               
                          
 
                    
 
                                                   
*/
/*                                                                            */
static int
wext_set_power(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, IN struct iw_param *prPower, IN char *pcExtra)
{
#if 1
	PARAM_POWER_MODE ePowerMode;
	INT_32 i4PowerValue;

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prPower);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prPower)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                                                        */
	/*                                                     */

	if (prPower->disabled) {
		ePowerMode = Param_PowerModeCAM;
	} else {
		i4PowerValue = prPower->value;
#if WIRELESS_EXT < 21
		i4PowerValue /= 1000000;
#endif
		if (i4PowerValue == 0) {
			ePowerMode = Param_PowerModeCAM;
		} else if (i4PowerValue == 1) {
			ePowerMode = Param_PowerModeMAX_PSP;
		} else if (i4PowerValue == 2) {
			ePowerMode = Param_PowerModeFast_PSP;
		} else {
			DBGLOG(INIT, INFO, ("%s(): unsupported power management mode value = %d.\n",
					    __func__, prPower->value));

			return -EINVAL;
		}
	}


	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSet802dot11PowerSaveProfile,
			   &ePowerMode, sizeof(ePowerMode), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS) {
		/*                                                                                       */
		return -EFAULT;
	}
#endif
	return 0;
}				/*                */


/*                                                                            */
/* 
                                 
 
                                        
                            
                                                                                
                          
 
                    
 
                                                         
*/
/*                                                                            */
static int
wext_get_power(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwrInfo, OUT struct iw_param *prPower, IN char *pcExtra)
{

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;
	PARAM_POWER_MODE ePowerMode = Param_PowerModeCAM;

	ASSERT(prNetDev);
	ASSERT(prPower);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prPower)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

#if 0
#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidQuery802dot11PowerSaveProfile,
			       &ePowerMode, sizeof(ePowerMode), TRUE, TRUE, &u4BufLen);
#else
	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQuery802dot11PowerSaveProfile,
				       &ePowerMode, sizeof(ePowerMode), &u4BufLen);
#endif
#else
	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQuery802dot11PowerSaveProfile,
				       &ePowerMode, sizeof(ePowerMode), &u4BufLen);
#endif

	if (rStatus != WLAN_STATUS_SUCCESS) {
		return -EFAULT;
	}

	prPower->value = 0;
	prPower->disabled = 1;

	if (Param_PowerModeCAM == ePowerMode) {
		prPower->value = 0;
		prPower->disabled = 1;
	} else if (Param_PowerModeMAX_PSP == ePowerMode) {
		prPower->value = 1;
		prPower->disabled = 0;
	} else if (Param_PowerModeFast_PSP == ePowerMode) {
		prPower->value = 2;
		prPower->disabled = 0;
	}

	prPower->flags = IW_POWER_PERIOD | IW_POWER_RELATIVE;
#if WIRELESS_EXT < 21
	prPower->value *= 1000000;
#endif

	/*                                                                        */
	/*                                                     */

	return 0;
}				/*                */

/*                                                                            */
/* 
                                          
 
                                        
                            
                                                                                        
                                                  
 
                    
                                       
                                                    
                                             
 
                                               
*/
/*                                                                            */
static int
wext_set_auth(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwrInfo, IN struct iw_param *prAuth, IN char *pcExtra)
{
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(prNetDev);
	ASSERT(prAuth);
	if (FALSE == GLUE_CHK_PR2(prNetDev, prAuth)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	/*                                                                   */
	switch (prAuth->flags & IW_AUTH_INDEX) {
	case IW_AUTH_WPA_VERSION:
#if CFG_SUPPORT_WAPI
		if (wlanQueryWapiMode(prGlueInfo->prAdapter)) {
			prGlueInfo->rWpaInfo.u4WpaVersion = IW_AUTH_WPA_VERSION_DISABLED;
			prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_OPEN_SYSTEM;
		} else {
			prGlueInfo->rWpaInfo.u4WpaVersion = prAuth->value;
		}
#else
		prGlueInfo->rWpaInfo.u4WpaVersion = prAuth->value;
#endif
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		prGlueInfo->rWpaInfo.u4CipherPairwise = prAuth->value;
		break;

	case IW_AUTH_CIPHER_GROUP:
		prGlueInfo->rWpaInfo.u4CipherGroup = prAuth->value;
		break;

	case IW_AUTH_KEY_MGMT:
		prGlueInfo->rWpaInfo.u4KeyMgmt = prAuth->value;
#if CFG_SUPPORT_WAPI
		if (prGlueInfo->rWpaInfo.u4KeyMgmt == IW_AUTH_KEY_MGMT_WAPI_PSK ||
		    prGlueInfo->rWpaInfo.u4KeyMgmt == IW_AUTH_KEY_MGMT_WAPI_CERT) {
			UINT_32 u4BufLen;
			WLAN_STATUS rStatus;

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetWapiMode,
					   &prAuth->value,
					   sizeof(UINT_32), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
			DBGLOG(INIT, INFO, ("IW_AUTH_WAPI_ENABLED :%d\n", prAuth->value));
		}
#endif
		if (prGlueInfo->rWpaInfo.u4KeyMgmt == IW_AUTH_KEY_MGMT_WPS)
			prGlueInfo->fgWpsActive = TRUE;
		else
			prGlueInfo->fgWpsActive = FALSE;
		break;

	case IW_AUTH_80211_AUTH_ALG:
		prGlueInfo->rWpaInfo.u4AuthAlg = prAuth->value;
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		prGlueInfo->rWpaInfo.fgPrivacyInvoke = prAuth->value;
		break;
#if CFG_SUPPORT_802_11W
	case IW_AUTH_MFP:
		/*                                                          */
		prGlueInfo->rWpaInfo.u4Mfp = prAuth->value;
		break;
#endif
#if CFG_SUPPORT_WAPI
	case IW_AUTH_WAPI_ENABLED:
		{
			UINT_32 u4BufLen;
			WLAN_STATUS rStatus;

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetWapiMode,
					   &prAuth->value,
					   sizeof(UINT_32), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		}
		DBGLOG(INIT, INFO, ("IW_AUTH_WAPI_ENABLED :%d\n", prAuth->value));
		break;
#endif
	default:
		/*
                                                                               
   */
		break;
	}
	return 0;
}				/*               */


/*                                                                            */
/* 
                                          
 
                                        
                            
                                                                                 
                                                  
 
                    
                                       
                                                    
                                             
 
                                               
*/
/*                                                                            */
#if CFG_SUPPORT_WAPI
UINT_8 keyStructBuf[320];	/*                              */
#else
UINT_8 keyStructBuf[100];	/*                              */
#endif

static int
wext_set_encode_ext(IN struct net_device *prNetDev,
		    IN struct iw_request_info *prIwrInfo,
		    IN struct iw_point *prEnc, IN char *pcExtra)
{
	P_PARAM_REMOVE_KEY_T prRemoveKey = (P_PARAM_REMOVE_KEY_T) keyStructBuf;
	P_PARAM_KEY_T prKey = (P_PARAM_KEY_T) keyStructBuf;


	P_PARAM_WEP_T prWepKey = (P_PARAM_WEP_T) wepBuf;

	struct iw_encode_ext *prIWEncExt = (struct iw_encode_ext *)pcExtra;

	ENUM_PARAM_ENCRYPTION_STATUS_T eEncStatus;
	ENUM_PARAM_AUTH_MODE_T eAuthMode;
	/*                                                      */

#if CFG_SUPPORT_WAPI
	P_PARAM_WPI_KEY_T prWpiKey = (P_PARAM_WPI_KEY_T) keyStructBuf;
#endif

	P_GLUE_INFO_T prGlueInfo = NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	ASSERT(prEnc);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prEnc, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	memset(keyStructBuf, 0, sizeof(keyStructBuf));

#if CFG_SUPPORT_WAPI
	if (prIWEncExt->alg == IW_ENCODE_ALG_SMS4) {
		if (prEnc->flags & IW_ENCODE_DISABLED) {
			/*                                                  */
			return 0;
		}
		/*       */
		prWpiKey->ucKeyID = (prEnc->flags & IW_ENCODE_INDEX);
		prWpiKey->ucKeyID--;
		if (prWpiKey->ucKeyID > 1) {
			/*                        */
			/*                                                                                   */
			return -EINVAL;
		}

		if (prIWEncExt->key_len != 32) {
			/*                      */
			/*                                                                                      */
			return -EINVAL;
		}
		/*                                                                                    */

		if (prIWEncExt->ext_flags & IW_ENCODE_EXT_GROUP_KEY) {
			prWpiKey->eKeyType = ENUM_WPI_GROUP_KEY;
			prWpiKey->eDirection = ENUM_WPI_RX;
		} else if (prIWEncExt->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			prWpiKey->eKeyType = ENUM_WPI_PAIRWISE_KEY;
			prWpiKey->eDirection = ENUM_WPI_RX_TX;
		}

		/*    */
		memcpy(prWpiKey->aucPN, prIWEncExt->tx_seq, IW_ENCODE_SEQ_MAX_SIZE * 2);

		/*       */
		memcpy(prWpiKey->aucAddrIndex, prIWEncExt->addr.sa_data, 6);

		memcpy(prWpiKey->aucWPIEK, prIWEncExt->key, 16);
		prWpiKey->u4LenWPIEK = 16;

		memcpy(prWpiKey->aucWPICK, &prIWEncExt->key[16], 16);
		prWpiKey->u4LenWPICK = 16;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetWapiKey,
				   prWpiKey,
				   sizeof(PARAM_WPI_KEY_T), FALSE, FALSE, TRUE, FALSE, &u4BufLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			/*                                                          */
		}

	} else
#endif
	{

		if ((prEnc->flags & IW_ENCODE_MODE) == IW_ENCODE_DISABLED) {
			prRemoveKey->u4Length = sizeof(*prRemoveKey);
			memcpy(prRemoveKey->arBSSID, prIWEncExt->addr.sa_data, 6);
			/*
                                                              
                                                          
    */

			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetRemoveKey,
					   prRemoveKey,
					   prRemoveKey->u4Length,
					   FALSE, FALSE, TRUE, FALSE, &u4BufLen);


			if (rStatus != WLAN_STATUS_SUCCESS) {
				DBGLOG(INIT, INFO, ("remove key error:%lx\n", rStatus));
			}
			return 0;
		}
		/*           */
		/*                                       */

		switch (prIWEncExt->alg) {
		case IW_ENCODE_ALG_NONE:
			break;
		case IW_ENCODE_ALG_WEP:
			/*                               */
			/*                            */
			/*                                   */
			/*                                                   */
			/*                        */
			if (prIWEncExt->key_len == 5 || prIWEncExt->key_len == 13
			    || prIWEncExt->key_len == 16) {
				/*                                 */
				prWepKey->u4KeyIndex = (prEnc->flags & IW_ENCODE_INDEX) ?
				    (prEnc->flags & IW_ENCODE_INDEX) - 1 : 0;
				if (prWepKey->u4KeyIndex > 3) {
					/*                        */
					return -EINVAL;
				}
				prWepKey->u4KeyIndex |= 0x80000000;
				prWepKey->u4Length = 12 + prIWEncExt->key_len;
				prWepKey->u4KeyLength = prIWEncExt->key_len;
				/*                                                                     */
				kalMemCopy(prWepKey->aucKeyMaterial, prIWEncExt->key,
					   prIWEncExt->key_len);


				rStatus = kalIoctl(prGlueInfo,
						   wlanoidSetAddWep,
						   prWepKey,
						   prWepKey->u4Length,
						   FALSE, FALSE, TRUE, FALSE, &u4BufLen);

				if (rStatus != WLAN_STATUS_SUCCESS) {
					DBGLOG(INIT, INFO,
					       ("wlanoidSetAddWep fail 0x%lx\n", rStatus));
					return -EFAULT;
				}

				/*                       */
				prGlueInfo->rWpaInfo.u4AuthAlg = IW_AUTH_ALG_SHARED_KEY |
				    IW_AUTH_ALG_OPEN_SYSTEM;
				eAuthMode = AUTH_MODE_AUTO_SWITCH;

				rStatus = kalIoctl(prGlueInfo,
						   wlanoidSetAuthMode,
						   &eAuthMode,
						   sizeof(eAuthMode),
						   FALSE, FALSE, FALSE, FALSE, &u4BufLen);

				if (rStatus != WLAN_STATUS_SUCCESS) {
					DBGLOG(INIT, INFO,
					       ("wlanoidSetAuthMode fail 0x%lx\n", rStatus));
					return -EFAULT;
				}

				prGlueInfo->rWpaInfo.u4CipherPairwise =
				    IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40;
				prGlueInfo->rWpaInfo.u4CipherGroup =
				    IW_AUTH_CIPHER_WEP104 | IW_AUTH_CIPHER_WEP40;

				eEncStatus = ENUM_WEP_ENABLED;


				rStatus = kalIoctl(prGlueInfo,
						   wlanoidSetEncryptionStatus,
						   &eEncStatus,
						   sizeof(ENUM_PARAM_ENCRYPTION_STATUS_T),
						   FALSE, FALSE, FALSE, FALSE, &u4BufLen);

				if (rStatus != WLAN_STATUS_SUCCESS) {
					DBGLOG(INIT, INFO,
					       ("wlanoidSetEncryptionStatus fail 0x%lx\n",
						rStatus));
					return -EFAULT;
				}

			} else {
				DBGLOG(INIT, INFO, ("key length %x\n", prIWEncExt->key_len));
				DBGLOG(INIT, INFO, ("key error\n"));
			}

			break;
		case IW_ENCODE_ALG_TKIP:
		case IW_ENCODE_ALG_CCMP:
#if CFG_SUPPORT_802_11W
		case IW_ENCODE_ALG_AES_CMAC:
#endif
			{

				/*       */
				prKey->u4KeyIndex = (prEnc->flags & IW_ENCODE_INDEX) ?
				    (prEnc->flags & IW_ENCODE_INDEX) - 1 : 0;
#if CFG_SUPPORT_802_11W
				if (prKey->u4KeyIndex > 5)
#else
				if (prKey->u4KeyIndex > 3)
#endif
				{
					DBGLOG(INIT, INFO,
					       ("key index error:0x%lx\n", prKey->u4KeyIndex));
					/*                        */
					return -EINVAL;
				}

				/*                                                       */
				/*                */
				if (prIWEncExt->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
					prKey->u4KeyIndex |= 0x1UL << 31;
				}

				/*                      */
				if (prIWEncExt->ext_flags & IW_ENCODE_EXT_GROUP_KEY) {
					/*           */
				} else {
					/*              */
					prKey->u4KeyIndex |= 0x1UL << 30;
				}

			}
			/*               */
			if (prIWEncExt->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
				prKey->u4KeyIndex |= 0x1UL << 29;
				memcpy(&prKey->rKeyRSC, prIWEncExt->rx_seq, IW_ENCODE_SEQ_MAX_SIZE);
			}

			/*       */
			memcpy(prKey->arBSSID, prIWEncExt->addr.sa_data, 6);

			/*                              */
			if (prIWEncExt->alg == IW_ENCODE_ALG_TKIP && prIWEncExt->key_len == 32) {
				memcpy(prKey->aucKeyMaterial, prIWEncExt->key, 16);
				memcpy(((PUINT_8) prKey->aucKeyMaterial) + 16, prIWEncExt->key + 24,
				       8);
				memcpy((prKey->aucKeyMaterial) + 24, prIWEncExt->key + 16, 8);
			} else {
				memcpy(prKey->aucKeyMaterial, prIWEncExt->key, prIWEncExt->key_len);
			}

			prKey->u4KeyLength = prIWEncExt->key_len;
			prKey->u4Length =
			    ((UINT_32) &(((P_PARAM_KEY_T) 0)->aucKeyMaterial)) +
			    prKey->u4KeyLength;


			rStatus = kalIoctl(prGlueInfo,
					   wlanoidSetAddKey,
					   prKey,
					   prKey->u4Length, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

			if (rStatus != WLAN_STATUS_SUCCESS) {
				DBGLOG(INIT, INFO, ("add key error:%lx\n", rStatus));
				return -EFAULT;
			}
			break;
		}
	}

	return 0;
}				/*                     */


/*                                                                            */
/* 
                         
 
                                        
                            
                                                   
                          
 
                        
                                                   
 
                                                             
*/
/*                                                                            */
static int wext_set_country(IN struct net_device *prNetDev, IN struct iwreq *iwr)
{
	P_GLUE_INFO_T prGlueInfo;
	WLAN_STATUS rStatus;
	UINT_32 u4BufLen;
	UINT_8 aucCountry[2];

	ASSERT(prNetDev);

	/*                                                              
                    
  */
	if (FALSE == GLUE_CHK_PR2(prNetDev, iwr) || !iwr->u.data.pointer || iwr->u.data.length < 10) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	aucCountry[0] = *((PUINT_8) iwr->u.data.pointer + 8);
	aucCountry[1] = *((PUINT_8) iwr->u.data.pointer + 9);

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidSetCountryCode,
			   &aucCountry[0], 2, FALSE, FALSE, TRUE, FALSE, &u4BufLen);

	return 0;
}

/*                                                                            */
/* 
                                                     
 
                                        
                                                                        
                                                        
 
                          
                                                  
                                             
                                              
 
       
*/
/*                                                                            */
int wext_support_ioctl(IN struct net_device *prDev, IN struct ifreq *prIfReq, IN int i4Cmd)
{
	/*                                                          */
	struct iwreq *iwr = (struct iwreq *)prIfReq;
	struct iw_request_info rIwReqInfo;
	int ret = 0;
	char *prExtraBuf = NULL;
	UINT_32 u4ExtraSize = 0;

	/*                                                        */

	/*                                                            */

	/*                  */
	rIwReqInfo.cmd = (__u16) i4Cmd;
	rIwReqInfo.flags = 0;

	switch (i4Cmd) {
	case SIOCGIWNAME:	/*                                    */
		ret = wext_get_name(prDev, &rIwReqInfo, (char *)&iwr->u.name, NULL);
		break;

		/*                                      */
		/*                                      */

	case SIOCSIWFREQ:	/*                     */
		ret = wext_set_freq(prDev, NULL, &iwr->u.freq, NULL);
		break;

	case SIOCGIWFREQ:	/*                     */
		ret = wext_get_freq(prDev, NULL, &iwr->u.freq, NULL);
		break;

	case SIOCSIWMODE:	/*                            */
		ret = wext_set_mode(prDev, NULL, &iwr->u.mode, NULL);
		/*          */
		break;

	case SIOCGIWMODE:	/*                            */
		ret = wext_get_mode(prDev, NULL, &iwr->u.mode, NULL);
		break;

		/*                                       */
		/*                                       */

		/*                                   */
	case SIOCGIWRANGE:	/*                                 */
		if (iwr->u.data.pointer != NULL) {
			/*                                    */
			if (iwr->u.data.length < sizeof(struct iw_range)) {
				ret = -E2BIG;
				break;
			}

			prExtraBuf = kalMemAlloc(sizeof(struct iw_range), VIR_MEM_TYPE);
			if (!prExtraBuf) {
				ret = -ENOMEM;
				break;
			}

			/*                  */
			memset(prExtraBuf, 0, sizeof(struct iw_range));
			iwr->u.data.length = sizeof(struct iw_range);

			ret = wext_get_range(prDev, NULL, &iwr->u.data, prExtraBuf);
			/*                       */
			if (copy_to_user(iwr->u.data.pointer, prExtraBuf, iwr->u.data.length)) {
				ret = -EFAULT;
			}

			kalMemFree(prExtraBuf, VIR_MEM_TYPE, sizeof(struct iw_range));
			prExtraBuf = NULL;
		} else {
			ret = -EINVAL;
		}
		break;

	case SIOCSIWPRIV:	/*                 */
		ret = wext_set_country(prDev, iwr);
		break;

		/*                                                      */
		/*                                   */
		/*                   
                                                                           
                                                            
   */
		/*                                      */
		/*                                      */
		/*                                         */
		/*                                         */

	case SIOCSIWAP:	/*                                                */
		if (iwr->u.ap_addr.sa_data[0] == 0 &&
		    iwr->u.ap_addr.sa_data[1] == 0 &&
		    iwr->u.ap_addr.sa_data[2] == 0 &&
		    iwr->u.ap_addr.sa_data[3] == 0 &&
		    iwr->u.ap_addr.sa_data[4] == 0 && iwr->u.ap_addr.sa_data[5] == 0) {
			/*                                        
                                                                   
    */
			ret = 0;
			break;
		} else {
			ret = wext_set_ap(prDev, NULL, &iwr->u.ap_addr, NULL);
		}
		break;

	case SIOCGIWAP:	/*                                                */
		ret = wext_get_ap(prDev, NULL, &iwr->u.ap_addr, NULL);
		break;

	case SIOCSIWMLME:	/*                                */
		/*                        */
		if (iwr->u.data.length != sizeof(struct iw_mlme)) {
			DBGLOG(INIT, INFO, ("MLME buffer strange:%d\n", iwr->u.data.length));
			ret = -EINVAL;
			break;
		}

		if (!iwr->u.data.pointer) {
			ret = -EINVAL;
			break;
		}

		prExtraBuf = kalMemAlloc(sizeof(struct iw_mlme), VIR_MEM_TYPE);
		if (!prExtraBuf) {
			ret = -ENOMEM;
			break;
		}

		if (copy_from_user(prExtraBuf, iwr->u.data.pointer, sizeof(struct iw_mlme))) {
			ret = -EFAULT;
		} else {
			ret = wext_set_mlme(prDev, NULL, &(iwr->u.data), prExtraBuf);
		}

		kalMemFree(prExtraBuf, VIR_MEM_TYPE, sizeof(struct iw_mlme));
		prExtraBuf = NULL;
		break;

		/*                                        */
	case SIOCSIWSCAN:	/*                      */
		if (iwr->u.data.pointer == NULL) {
			ret = wext_set_scan(prDev, NULL, NULL, NULL);
		}
#if WIRELESS_EXT > 17
		else if (iwr->u.data.length == sizeof(struct iw_scan_req)) {
			prExtraBuf = kalMemAlloc(MAX_SSID_LEN, VIR_MEM_TYPE);
			if (!prExtraBuf) {
				ret = -ENOMEM;
				break;
			}
			if (copy_from_user
			    (prExtraBuf, ((struct iw_scan_req *)(iwr->u.data.pointer))->essid,
			     ((struct iw_scan_req *)(iwr->u.data.pointer))->essid_len)) {
				ret = -EFAULT;
			} else {
				ret =
				    wext_set_scan(prDev, NULL, (union iwreq_data *)&(iwr->u.data),
						  prExtraBuf);
			}

			kalMemFree(prExtraBuf, VIR_MEM_TYPE, MAX_SSID_LEN);
			prExtraBuf = NULL;
		}
#endif
		else {
			ret = -EINVAL;
		}
		break;
#if 1
	case SIOCGIWSCAN:	/*                          */
		if (!iwr->u.data.pointer || !iwr->u.essid.pointer) {
			ret = -EINVAL;
			break;
		}

		u4ExtraSize = iwr->u.data.length;
		/*                                                                */
		prExtraBuf = kalMemAlloc(u4ExtraSize, VIR_MEM_TYPE);
		if (!prExtraBuf) {
			ret = -ENOMEM;
			break;
		}

		/*                                                      */
		ret = wext_get_scan(prDev, NULL, &iwr->u.data, prExtraBuf);
		if (ret != 0) {
			if (ret == -E2BIG) {
				DBGLOG(INIT, INFO, ("[wifi] wext_get_scan -E2BIG\n"));
			}
		} else {
			/*                               */
			ASSERT(iwr->u.data.length <= u4ExtraSize);
			if (iwr->u.data.length > u4ExtraSize) {
				DBGLOG(INIT, INFO,
				       ("Updated result length is larger than allocated (%d > %ld)\n",
					iwr->u.data.length, u4ExtraSize));
				iwr->u.data.length = u4ExtraSize;
			}

			if (copy_to_user(iwr->u.data.pointer, prExtraBuf, iwr->u.data.length)) {
				ret = -EFAULT;
			}
		}

		kalMemFree(prExtraBuf, VIR_MEM_TYPE, u4ExtraSize);
		prExtraBuf = NULL;

		break;

#endif

#if 1
	case SIOCSIWESSID:	/*                                 */
		if (iwr->u.essid.length > IW_ESSID_MAX_SIZE) {
			ret = -E2BIG;
			break;
		}
		if (!iwr->u.essid.pointer) {
			ret = -EINVAL;
			break;
		}

		prExtraBuf = kalMemAlloc(IW_ESSID_MAX_SIZE + 4, VIR_MEM_TYPE);
		if (!prExtraBuf) {
			ret = -ENOMEM;
			break;
		}

		if (copy_from_user(prExtraBuf, iwr->u.essid.pointer, iwr->u.essid.length)) {
			ret = -EFAULT;
		} else {
			/*                              */
			/*                                      */
			/*                                                                                 */
			ret = wext_set_essid(prDev, NULL, &iwr->u.essid, prExtraBuf);
			/*                                 */
		}

		kalMemFree(prExtraBuf, VIR_MEM_TYPE, IW_ESSID_MAX_SIZE + 4);
		prExtraBuf = NULL;
		break;

#endif

	case SIOCGIWESSID:	/*                  */
		if (!iwr->u.essid.pointer) {
			ret = -EINVAL;
			break;
		}

		if (iwr->u.essid.length < IW_ESSID_MAX_SIZE) {
			DBGLOG(INIT, INFO, ("[wifi] iwr->u.essid.length:%d too small\n",
					    iwr->u.essid.length));
			ret = -E2BIG;	/*                              */
			break;
		}

		prExtraBuf = kalMemAlloc(IW_ESSID_MAX_SIZE, VIR_MEM_TYPE);
		if (!prExtraBuf) {
			ret = -ENOMEM;
			break;
		}

		/*                                                    */

		ret = wext_get_essid(prDev, NULL, &iwr->u.essid, prExtraBuf);
		if (ret == 0) {
			if (copy_to_user(iwr->u.essid.pointer, prExtraBuf, iwr->u.essid.length)) {
				ret = -EFAULT;
			}
		}

		kalMemFree(prExtraBuf, VIR_MEM_TYPE, IW_ESSID_MAX_SIZE);
		prExtraBuf = NULL;

		break;

		/*                                          */
		/*                                          */

	case SIOCSIWRATE:	/*                                    */
		/*                                                                 */
		break;

	case SIOCGIWRATE:	/*                                    */
		ret = wext_get_rate(prDev, NULL, &iwr->u.bitrate, NULL);
		break;

	case SIOCSIWRTS:	/*                               */
		ret = wext_set_rts(prDev, NULL, &iwr->u.rts, NULL);
		break;

	case SIOCGIWRTS:	/*                               */
		ret = wext_get_rts(prDev, NULL, &iwr->u.rts, NULL);
		break;

		/*                                       */
	case SIOCGIWFRAG:	/*                            */
		ret = wext_get_frag(prDev, NULL, &iwr->u.frag, NULL);
		break;

	case SIOCSIWTXPOW:	/*                                      */
		ret = wext_set_txpow(prDev, NULL, &iwr->u.txpower, NULL);
		break;

	case SIOCGIWTXPOW:	/*                                      */
		ret = wext_get_txpow(prDev, NULL, &iwr->u.txpower, NULL);
		break;

		/*                                        */
		/*                                        */

#if 1
	case SIOCSIWENCODE:	/*                                   */
		/*                                                     */
		if (iwr->u.encoding.pointer) {
			if (iwr->u.encoding.length > 16) {
				ret = -E2BIG;
				break;
			}

			u4ExtraSize = iwr->u.encoding.length;
			prExtraBuf = kalMemAlloc(u4ExtraSize, VIR_MEM_TYPE);
			if (!prExtraBuf) {
				ret = -ENOMEM;
				break;
			}

			if (copy_from_user(prExtraBuf,
					   iwr->u.encoding.pointer, iwr->u.encoding.length)) {
				ret = -EFAULT;
			}
		} else if (iwr->u.encoding.length != 0) {
			ret = -EINVAL;
			break;
		}

		if (ret == 0) {
			ret = wext_set_encode(prDev, NULL, &iwr->u.encoding, prExtraBuf);
		}

		if (prExtraBuf) {
			kalMemFree(prExtraBuf, VIR_MEM_TYPE, u4ExtraSize);
			prExtraBuf = NULL;
		}
		break;

	case SIOCGIWENCODE:	/*                                   */
		/*               */
		ret = wext_get_encode(prDev, NULL, &iwr->u.encoding, NULL);
		break;

	case SIOCSIWPOWER:	/*                              */
		ret = wext_set_power(prDev, NULL, &iwr->u.power, NULL);
		break;

	case SIOCGIWPOWER:	/*                              */
		ret = wext_get_power(prDev, NULL, &iwr->u.power, NULL);
		break;

#if WIRELESS_EXT > 17
	case SIOCSIWGENIE:	/*                    */
		if (iwr->u.data.pointer) {
			P_GLUE_INFO_T prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
			if (1 /*                                          */) {
				/*                        */
#if CFG_SUPPORT_WAPI
				if (iwr->u.data.length > 42 /*                        */) {
					ret = -EINVAL;
					break;
				}
#endif
				u4ExtraSize = iwr->u.data.length;
				if (u4ExtraSize) {
					prExtraBuf = kalMemAlloc(u4ExtraSize, VIR_MEM_TYPE);
					if (!prExtraBuf) {
						ret = -ENOMEM;
						break;
					}
					if (copy_from_user(prExtraBuf,
							   iwr->u.data.pointer,
							   iwr->u.data.length)) {
						ret = -EFAULT;
					} else {
						WLAN_STATUS rStatus;
						UINT_32 u4BufLen;
#if CFG_SUPPORT_WAPI
						rStatus = kalIoctl(prGlueInfo,
								   wlanoidSetWapiAssocInfo,
								   prExtraBuf,
								   u4ExtraSize,
								   FALSE,
								   FALSE, TRUE, FALSE, &u4BufLen);

						if (rStatus != WLAN_STATUS_SUCCESS) {
							/*                                                                      */
#endif
#if CFG_SUPPORT_WPS2
							PUINT_8 prDesiredIE = NULL;
							if (wextSrchDesiredWPSIE(prExtraBuf,
										 u4ExtraSize,
										 0xDD,
										 (PUINT_8 *) &
										 prDesiredIE)) {
								rStatus =
								    kalIoctl(prGlueInfo,
									     wlanoidSetWSCAssocInfo,
									     prDesiredIE,
									     IE_SIZE(prDesiredIE),
									     FALSE, FALSE, TRUE,
									     FALSE, &u4BufLen);
								if (rStatus != WLAN_STATUS_SUCCESS) {
									/*                                                                    */
								}
							}
#endif
#if CFG_SUPPORT_WAPI
						}
#endif
					}
					kalMemFree(prExtraBuf, VIR_MEM_TYPE, u4ExtraSize);
					prExtraBuf = NULL;
				}
			}
		}
		break;

	case SIOCGIWGENIE:	/*                            */
		break;

#endif

	case SIOCSIWAUTH:	/*                              */
		ret = wext_set_auth(prDev, NULL, &iwr->u.param, NULL);
		break;

		/*                                   */
	case SIOCSIWENCODEEXT:	/*                                            */
		if (iwr->u.encoding.pointer) {
			u4ExtraSize = iwr->u.encoding.length;
			prExtraBuf = kalMemAlloc(u4ExtraSize, VIR_MEM_TYPE);
			if (!prExtraBuf) {
				ret = -ENOMEM;
				break;
			}

			if (copy_from_user(prExtraBuf,
					   iwr->u.encoding.pointer, iwr->u.encoding.length)) {
				ret = -EFAULT;
			}
		} else if (iwr->u.encoding.length != 0) {
			ret = -EINVAL;
			break;
		}

		if (ret == 0) {
			ret = wext_set_encode_ext(prDev, NULL, &iwr->u.encoding, prExtraBuf);
		}

		if (prExtraBuf) {
			kalMemFree(prExtraBuf, VIR_MEM_TYPE, u4ExtraSize);
			prExtraBuf = NULL;
		}
		break;

		/*                                        */

	case SIOCSIWPMKSA:	/*                               */
#if 1
		if (iwr->u.data.pointer) {
			/*                        */
			if (iwr->u.data.length != sizeof(struct iw_pmksa)) {
				ret = -EINVAL;
				break;
			}

			u4ExtraSize = sizeof(struct iw_pmksa);
			prExtraBuf = kalMemAlloc(u4ExtraSize, VIR_MEM_TYPE);
			if (!prExtraBuf) {
				ret = -ENOMEM;
				break;
			}

			if (copy_from_user(prExtraBuf,
					   iwr->u.data.pointer, sizeof(struct iw_pmksa))) {
				ret = -EFAULT;
			} else {
				switch (((struct iw_pmksa *)prExtraBuf)->cmd) {
				case IW_PMKSA_ADD:
					/*
                                                       
                                                                
      */
					{
						P_GLUE_INFO_T prGlueInfo =
						    *((P_GLUE_INFO_T *) netdev_priv(prDev));
						WLAN_STATUS rStatus;
						UINT_32 u4BufLen;
						P_PARAM_PMKID_T prPmkid;

						prPmkid =
						    (P_PARAM_PMKID_T) kalMemAlloc(8 +
										  sizeof
										  (PARAM_BSSID_INFO_T),
										  VIR_MEM_TYPE);
						if (!prPmkid) {
							DBGLOG(INIT, INFO,
							       ("Can not alloc memory for IW_PMKSA_ADD\n"));
							ret = -ENOMEM;
							break;
						}

						prPmkid->u4Length = 8 + sizeof(PARAM_BSSID_INFO_T);
						prPmkid->u4BSSIDInfoCount = 1;
						kalMemCopy(prPmkid->arBSSIDInfo->arBSSID,
							   ((struct iw_pmksa *)prExtraBuf)->bssid.
							   sa_data, 6);
						kalMemCopy(prPmkid->arBSSIDInfo->arPMKID,
							   ((struct iw_pmksa *)prExtraBuf)->pmkid,
							   IW_PMKID_LEN);

						rStatus = kalIoctl(prGlueInfo,
								   wlanoidSetPmkid,
								   prPmkid,
								   sizeof(PARAM_PMKID_T),
								   FALSE,
								   FALSE, TRUE, FALSE, &u4BufLen);

						if (rStatus != WLAN_STATUS_SUCCESS) {
							DBGLOG(INIT, INFO,
							       ("add pmkid error:%lx\n", rStatus));
						}
						kalMemFree(prPmkid, VIR_MEM_TYPE,
							   8 + sizeof(PARAM_BSSID_INFO_T));
					}
					break;
				case IW_PMKSA_REMOVE:
					/*
                                                          
                                                          
      */
					break;
				case IW_PMKSA_FLUSH:
					/*
                                             
      */
					{
						P_GLUE_INFO_T prGlueInfo =
						    *((P_GLUE_INFO_T *) netdev_priv(prDev));
						WLAN_STATUS rStatus;
						UINT_32 u4BufLen;
						P_PARAM_PMKID_T prPmkid;

						prPmkid =
						    (P_PARAM_PMKID_T) kalMemAlloc(8, VIR_MEM_TYPE);
						if (!prPmkid) {
							DBGLOG(INIT, INFO,
							       ("Can not alloc memory for IW_PMKSA_FLUSH\n"));
							ret = -ENOMEM;
							break;
						}

						prPmkid->u4Length = 8;
						prPmkid->u4BSSIDInfoCount = 0;

						rStatus = kalIoctl(prGlueInfo,
								   wlanoidSetPmkid,
								   prPmkid,
								   sizeof(PARAM_PMKID_T),
								   FALSE,
								   FALSE, TRUE, FALSE, &u4BufLen);

						if (rStatus != WLAN_STATUS_SUCCESS) {
							DBGLOG(INIT, INFO,
							       ("flush pmkid error:%lx\n",
								rStatus));
						}
						kalMemFree(prPmkid, VIR_MEM_TYPE, 8);
					}
					break;
				default:
					DBGLOG(INIT, INFO, ("UNKNOWN iw_pmksa command:%d\n",
							    ((struct iw_pmksa *)prExtraBuf)->cmd));
					ret = -EFAULT;
					break;
				}
			}

			if (prExtraBuf) {
				kalMemFree(prExtraBuf, VIR_MEM_TYPE, u4ExtraSize);
				prExtraBuf = NULL;
			}
		} else if (iwr->u.data.length != 0) {
			ret = -EINVAL;
			break;
		}
#endif
		break;

#endif

	default:
		/*                                                         */
		ret = -EOPNOTSUPP;
		break;
	}

	/*                                                                         */

	return ret;
}				/*                    */



/*                                                                            */
/* 
                                                                       
 
                                        
                                                                           
                                               
                                                  
 
                
 
                                                                                
                                                                         
                                               
*/
/*                                                                            */
void
wext_indicate_wext_event(IN P_GLUE_INFO_T prGlueInfo,
			 IN unsigned int u4Cmd,
			 IN unsigned char *pucData, IN unsigned int u4dataLen)
{
	union iwreq_data wrqu;
	unsigned char *pucExtraInfo = NULL;
#if WIRELESS_EXT >= 15
	unsigned char *pucDesiredIE = NULL;
	unsigned char aucExtraInfoBuf[200];
#endif
#if WIRELESS_EXT < 18
	int i;
#endif

	memset(&wrqu, 0, sizeof(wrqu));

	switch (u4Cmd) {
	case SIOCGIWTXPOW:
		memcpy(&wrqu.power, pucData, u4dataLen);
		break;
	case SIOCGIWSCAN:
		complete_all(&prGlueInfo->rScanComp);
		break;

	case SIOCGIWAP:
		if (pucData) {
			memcpy(&wrqu.ap_addr.sa_data, pucData, ETH_ALEN);
		} else {
			memset(&wrqu.ap_addr.sa_data, 0, ETH_ALEN);
		}
		break;

	case IWEVASSOCREQIE:
#if WIRELESS_EXT < 15
		/*                                            */
		goto skip_indicate_event;
#else
		/*                                                         */
		if (wextSrchDesiredWPAIE(pucData, u4dataLen, 0x30, &pucDesiredIE)) {
			/*              */
		}
#if 0
		else if (wextSrchDesiredWPSIE(pucData, u4dataLen, 0xDD, &pucDesiredIE)) {
			/*              */
		}
#endif
		else if (wextSrchDesiredWPAIE(pucData, u4dataLen, 0xDD, &pucDesiredIE)) {
			/*              */
		}
#if CFG_SUPPORT_WAPI		/*          */
		else if (wextSrchDesiredWAPIIE(pucData, u4dataLen, &pucDesiredIE)) {
			/*                                      */
			/*               */
		}
#endif
		else {
			/*                                      */
			goto skip_indicate_event;
		}

#if WIRELESS_EXT < 18
		/*                                          */
		u4Cmd = IWEVCUSTOM;
		pucExtraInfo = aucExtraInfoBuf;
		pucExtraInfo += sprintf(pucExtraInfo, "ASSOCINFO(ReqIEs=");
		/*                                                                                */
		/*                                                                  */
		for (i = 0; i < pucDesiredIE[1] + 2; ++i) {
			pucExtraInfo += sprintf(pucExtraInfo, "%02x", pucDesiredIE[i]);
		}
		pucExtraInfo = aucExtraInfoBuf;
		wrqu.data.length = 17 + (pucDesiredIE[1] + 2) * 2;
#else
		/*                                        */
		pucExtraInfo = pucDesiredIE;
		wrqu.data.length = pucDesiredIE[1] + 2;
#endif
#endif				/*                   */
		break;

	case IWEVMICHAELMICFAILURE:
#if WIRELESS_EXT < 15
		/*                                            */
		goto skip_indicate_event;
#else
		if (pucData) {
			P_PARAM_AUTH_REQUEST_T pAuthReq = (P_PARAM_AUTH_REQUEST_T) pucData;
			/*                                          */
			u4Cmd = IWEVCUSTOM;
			pucExtraInfo = aucExtraInfoBuf;
			pucExtraInfo += sprintf(pucExtraInfo, "MLME-MICHAELMICFAILURE.indication ");
			pucExtraInfo += sprintf(pucExtraInfo,
						"%s",
						(pAuthReq->u4Flags ==
						 PARAM_AUTH_REQUEST_GROUP_ERROR) ? "groupcast " :
						"unicast ");

			wrqu.data.length = pucExtraInfo - aucExtraInfoBuf;
			pucExtraInfo = aucExtraInfoBuf;
		}
#endif				/*                   */
		break;

	case IWEVPMKIDCAND:
		if (prGlueInfo->rWpaInfo.u4WpaVersion == IW_AUTH_WPA_VERSION_WPA2 &&
		    prGlueInfo->rWpaInfo.u4KeyMgmt == IW_AUTH_KEY_MGMT_802_1X) {

			/*                   */
#if WIRELESS_EXT >= 18
			P_PARAM_PMKID_CANDIDATE_T prPmkidCand = (P_PARAM_PMKID_CANDIDATE_T) pucData;

			struct iw_pmkid_cand rPmkidCand;
			pucExtraInfo = aucExtraInfoBuf;

			rPmkidCand.flags = prPmkidCand->u4Flags;
			rPmkidCand.index = 0;
			kalMemCopy(rPmkidCand.bssid.sa_data, prPmkidCand->arBSSID, 6);

			kalMemCopy(pucExtraInfo, (PUINT_8) &rPmkidCand,
				   sizeof(struct iw_pmkid_cand));
			wrqu.data.length = sizeof(struct iw_pmkid_cand);

			/*                                                */
			/*                               */
#else
			/*                                                             */
			goto skip_indicate_event;
#endif
		} else {
			/*                                                              */
			goto skip_indicate_event;
		}
		break;

	case IWEVCUSTOM:
		u4Cmd = IWEVCUSTOM;
		pucExtraInfo = aucExtraInfoBuf;
		kalMemCopy(pucExtraInfo, pucData, sizeof(PTA_IPC_T));
		wrqu.data.length = sizeof(PTA_IPC_T);
		break;

	default:
		/*                                                       */
		goto skip_indicate_event;
	}

	/*                          */
	wireless_send_event(prGlueInfo->prDevHandler, u4Cmd, &wrqu, pucExtraInfo);

 skip_indicate_event:
	return;
}				/*                          */


/*                                                                            */
/* 
                                                                                
                     
 
                                                                                
                                                                           
 
                                               
 
                                          
 
*/
/*                                                                            */
struct iw_statistics *wext_get_wireless_stats(struct net_device *prDev)
{

	WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
	P_GLUE_INFO_T prGlueInfo = NULL;
	struct iw_statistics *pStats = NULL;
	INT_32 i4Rssi;
	UINT_32 bufLen = 0;


	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		goto stat_out;
	}

	pStats = (struct iw_statistics *)(&(prGlueInfo->rIwStats));

	if (!prDev || !netif_carrier_ok(prDev)) {
		/*                       */
		goto stat_out;
	}

	rStatus = kalIoctl(prGlueInfo,
			   wlanoidQueryRssi,
			   &i4Rssi, sizeof(i4Rssi), TRUE, TRUE, TRUE, FALSE, &bufLen);

 stat_out:
	return pStats;
}				/*                         */

/*                                                                            */
/* 
                                                                    
 
                                           
                                                                           
                                                        
 
                        
                                                     
                           
 
*/
/*                                                                            */
int wext_get_priv(IN struct net_device *prNetDev, IN struct ifreq *prIfReq)
{
	/*                                                          */
	struct iwreq *prIwReq = (struct iwreq *)prIfReq;
	struct iw_point *prData = (struct iw_point *)&prIwReq->u.data;
	UINT_16 u2BufferSize = 0;

	u2BufferSize = prData->length;

	/*                               */
	prData->length = (__u16) sizeof(rIwPrivTable) / sizeof(struct iw_priv_args);

	if (u2BufferSize < prData->length) {
		return -E2BIG;
	}

	if (prData->length) {
		if (copy_to_user(prData->pointer, rIwPrivTable, sizeof(rIwPrivTable))) {
			return -EFAULT;
		}
	}

	return 0;
}				/*               */

#endif				/*              */
