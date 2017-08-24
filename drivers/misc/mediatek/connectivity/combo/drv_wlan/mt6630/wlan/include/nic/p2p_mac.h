/*
                                                                            
*/

/*                     
                              
  
                       
 */





#ifndef _P2P_MAC_H
#define _P2P_MAC_H

/*                                                                              
                                                      
                                                                                 
 */

/*                                                                              
                                                           
                                                                                 
 */

/*                                                                              
                                                 
                                                                                 
 */

#define ACTION_PUBLIC_WIFI_DIRECT                   9
#define ACTION_GAS_INITIAL_REQUEST                 10
#define ACTION_GAS_INITIAL_RESPONSE               11
#define ACTION_GAS_COMEBACK_REQUEST           12
#define ACTION_GAS_COMEBACK_RESPONSE         13


/*                                             */
#define P2P_PUBLIC_ACTION_GO_NEGO_REQ               0
#define P2P_PUBLIC_ACTION_GO_NEGO_RSP               1
#define P2P_PUBLIC_ACTION_GO_NEGO_CFM               2
#define P2P_PUBLIC_ACTION_INVITATION_REQ            3
#define P2P_PUBLIC_ACTION_INVITATION_RSP            4
#define P2P_PUBLIC_ACTION_DEV_DISCOVER_REQ          5
#define P2P_PUBLIC_ACTION_DEV_DISCOVER_RSP          6
#define P2P_PUBLIC_ACTION_PROV_DISCOVERY_REQ        7
#define P2P_PUBLIC_ACTION_PROV_DISCOVERY_RSP        8

/*                                     */
#define P2P_ACTION_NOTICE_OF_ABSENCE                0
#define P2P_ACTION_P2P_PRESENCE_REQ                 1
#define P2P_ACTION_P2P_PRESENCE_RSP                 2
#define P2P_ACTION_GO_DISCOVER_REQ                  3

#define P2P_PUBLIC_ACTION_FRAME_LEN                (WLAN_MAC_MGMT_HEADER_LEN + 8)
#define P2P_ACTION_FRAME_LEN                       (WLAN_MAC_MGMT_HEADER_LEN + 7)


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

/*                                                              */
/*                                   */
#define WSC_ATTRI_HDR_LEN                                   4	/*                                */
#define WSC_ATTRI_MAX_LEN_VERSION                           1
#define WSC_ATTRI_MAX_LEN_DEVICE_PASSWORD_ID                2
#define WSC_ATTRI_LEN_CONFIG_METHOD                         2


/*                                            */
/*                           */
#define P2P_OUI_TYPE_LEN                            4
#define P2P_IE_OUI_HDR                              (ELEM_HDR_LEN + P2P_OUI_TYPE_LEN)	/*                       
                                  */

/*                                   */
#define P2P_ATTRI_HDR_LEN                           3	/*                                */

/*                                          */
#define P2P_ATTRI_ID_STATUS                                 0
#define P2P_ATTRI_ID_REASON_CODE                            1
#define P2P_ATTRI_ID_P2P_CAPABILITY                         2
#define P2P_ATTRI_ID_P2P_DEV_ID                             3
#define P2P_ATTRI_ID_GO_INTENT                              4
#define P2P_ATTRI_ID_CFG_TIMEOUT                            5
#define P2P_ATTRI_ID_LISTEN_CHANNEL                         6
#define P2P_ATTRI_ID_P2P_GROUP_BSSID                        7
#define P2P_ATTRI_ID_EXT_LISTEN_TIMING                      8
#define P2P_ATTRI_ID_INTENDED_P2P_IF_ADDR                   9
#define P2P_ATTRI_ID_P2P_MANAGEABILITY                      10
#define P2P_ATTRI_ID_CHANNEL_LIST                           11
#define P2P_ATTRI_ID_NOTICE_OF_ABSENCE                      12
#define P2P_ATTRI_ID_P2P_DEV_INFO                           13
#define P2P_ATTRI_ID_P2P_GROUP_INFO                         14
#define P2P_ATTRI_ID_P2P_GROUP_ID                           15
#define P2P_ATTRI_ID_P2P_INTERFACE                          16
#define P2P_ATTRI_ID_OPERATING_CHANNEL                      17
#define P2P_ATTRI_ID_INVITATION_FLAG                        18
#define P2P_ATTRI_ID_VENDOR_SPECIFIC                        221

/*                                  */
#define P2P_ATTRI_MAX_LEN_STATUS                            1		/*   */
#define P2P_ATTRI_MAX_LEN_REASON_CODE                       1		/*   */
#define P2P_ATTRI_MAX_LEN_P2P_CAPABILITY                    2		/*   */
#define P2P_ATTRI_MAX_LEN_P2P_DEV_ID                        6		/*   */
#define P2P_ATTRI_MAX_LEN_GO_INTENT                         1		/*   */
#define P2P_ATTRI_MAX_LEN_CFG_TIMEOUT                       2		/*   */
#define P2P_ATTRI_MAX_LEN_LISTEN_CHANNEL                    5		/*   */
#define P2P_ATTRI_MAX_LEN_P2P_GROUP_BSSID                   6		/*   */
#define P2P_ATTRI_MAX_LEN_EXT_LISTEN_TIMING                 4		/*   */
#define P2P_ATTRI_MAX_LEN_INTENDED_P2P_IF_ADDR              6		/*   */
#define P2P_ATTRI_MAX_LEN_P2P_MANAGEABILITY                 1		/*    */
/*                                                                                      */ /*    */
#define P2P_ATTRI_LEN_CHANNEL_LIST                                  3	/*    */
#define P2P_ATTRI_LEN_CHANNEL_ENTRY                                  2	/*    */

#define P2P_MAXIMUM_ATTRIBUTE_LEN                   251


/*                                    */
#define P2P_STATUS_SUCCESS                                  0
#define P2P_STATUS_FAIL_INFO_IS_CURRENTLY_UNAVAILABLE   1
#define P2P_STATUS_FAIL_INCOMPATIBLE_PARAM                  2
#define P2P_STATUS_FAIL_LIMIT_REACHED                       3
#define P2P_STATUS_FAIL_INVALID_PARAM                       4
#define P2P_STATUS_FAIL_UNABLE_ACCOMMODATE_REQ              5
#define P2P_STATUS_FAIL_PREVIOUS_PROTOCOL_ERR               6
#define P2P_STATUS_FAIL_NO_COMMON_CHANNELS                  7
#define P2P_STATUS_FAIL_UNKNOWN_P2P_GROUP                   8
#define P2P_STATUS_FAIL_SAME_INTENT_VALUE_15                9
#define P2P_STATUS_FAIL_INCOMPATIBLE_PROVISION_METHOD       10
#define P2P_STATUS_FAIL_REJECTED_BY_USER                    11

/*                                                        */
#define P2P_CTW_OPPPS_PARAM_OPPPS_FIELD                     BIT(7)
#define P2P_CTW_OPPPS_PARAM_CTWINDOW_MASK                   BITS(0,6)


/*                                                           */

/*                                     */
typedef struct _IE_P2P_T {
	UINT_8	ucId;			/*            */
	UINT_8	ucLength;		/*        */
	UINT_8	aucOui[3];		/*     */
	UINT_8	ucOuiType;		/*          */
	UINT_8	aucP2PAttributes[1];	/*                */
} __KAL_ATTRIB_PACKED__ IE_P2P_T, *P_IE_P2P_T;

/*                                   */
typedef struct _WSC_ATTRIBUTE_T {
	UINT_16 u2Id;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	aucBody[1];	/*            */
} __KAL_ATTRIB_PACKED__ WSC_ATTRIBUTE_T, *P_WSC_ATTRIBUTE_T;


/*                                  */
typedef struct _P2P_ATTRI_STATUS_T {
	UINT_8	ucId;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	ucStatusCode;	/*             */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_STATUS_T, *P_P2P_ATTRI_STATUS_T;


/*                                               */
typedef struct _P2P_ATTRI_EXT_LISTEN_TIMING_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_16 u2AvailPeriod;		/*                     */
	UINT_16 u2AvailInterval;	/*                       */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_EXT_LISTEN_TIMING_T, *P_P2P_ATTRI_EXT_LISTEN_TIMING_T;


/*                                            */
typedef struct _P2P_PUBLIC_ACTION_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                              */
	UINT_8	ucCategory;			/*                */
	UINT_8	ucAction;			/*                    */
	UINT_8	aucOui[3];			/*                  */
	UINT_8	ucOuiType;			/*      */
	UINT_8	ucOuiSubtype;			/*                                                                   
                */
	UINT_8	ucDialogToken;			/*               */
	UINT_8	aucInfoElem[1];			/*                 */
} __KAL_ATTRIB_PACKED__ P2P_PUBLIC_ACTION_FRAME_T, *P_P2P_PUBLIC_ACTION_FRAME_T;


/*                                             */
typedef struct _P2P_ACTION_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                       */
	UINT_8	ucCategory;			/*      */
	UINT_8	aucOui[3];			/*                  */
	UINT_8	ucOuiType;			/*      */
	UINT_8	ucOuiSubtype;			/*  */
	UINT_8	ucDialogToken;
	UINT_8	aucInfoElem[1];
} __KAL_ATTRIB_PACKED__ P2P_ACTION_FRAME_T, *P_P2P_ACTION_FRAME_T;

/*                                                        */
typedef struct _GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                              */
	UINT_8	ucCategory;			/*                */
	UINT_8	ucAction;			/*                    */
	UINT_8	ucDialogToken;			/*               */
	UINT_8	aucInfoElem[1];			/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T,
*P_GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T;

/*                                                         */
typedef struct _GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                              */
	UINT_8	ucCategory;			/*                */
	UINT_8	ucAction;			/*                    */
	UINT_8	ucDialogToken;			/*               */
	UINT_16 u2StatusCode;			/*                   */
	UINT_16 u2ComebackDelay;		/*                   *//*                */
	UINT_8	aucInfoElem[1];			/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T,
*P_GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T;


/*                                                           */
typedef struct _GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                              */
	UINT_8	ucCategory;			/*                */
	UINT_8	ucAction;			/*                    */
	UINT_8	ucDialogToken;			/*               */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T,
*P_GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T;

/*                                                            */
typedef struct _GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;			/*               */
	UINT_16 u2Duration;			/*          */
	UINT_8	aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8	aucBSSID[MAC_ADDR_LEN];		/*       */
	UINT_16 u2SeqCtrl;			/*                  */
	/*                              */
	UINT_8	ucCategory;			/*                */
	UINT_8	ucAction;			/*                    */
	UINT_8	ucDialogToken;			/*               */
	UINT_16 u2StatusCode;			/*                    */
	UINT_8	ucFragmentID;			/*                   */
	UINT_16 u2ComebackDelay;		/*                    */
	UINT_8	aucInfoElem[1];			/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T,
*P_GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T;




typedef struct _P2P_SD_VENDER_SPECIFIC_CONTENT_T {
	/*                                            */
	UINT_8	ucOuiSubtype;	/*      */
	UINT_16 u2ServiceUpdateIndicator;
	UINT_8	aucServiceTLV[1];
} __KAL_ATTRIB_PACKED__ P2P_SD_VENDER_SPECIFIC_CONTENT_T, *P_P2P_SD_VENDER_SPECIFIC_CONTENT_T;


typedef struct _P2P_SERVICE_REQUEST_TLV_T {
	UINT_16 u2Length;
	UINT_8	ucServiceProtocolType;
	UINT_8	ucServiceTransID;
	UINT_8	aucQueryData[1];
} __KAL_ATTRIB_PACKED__ P2P_SERVICE_REQUEST_TLV_T, *P_P2P_SERVICE_REQUEST_TLV_T;



typedef struct _P2P_SERVICE_RESPONSE_TLV_T {
	UINT_16 u2Length;
	UINT_8	ucServiceProtocolType;
	UINT_8	ucServiceTransID;
	UINT_8	ucStatusCode;
	UINT_8	aucResponseData[1];
} __KAL_ATTRIB_PACKED__ P2P_SERVICE_RESPONSE_TLV_T, *P_P2P_SERVICE_RESPONSE_TLV_T;


/*                                   */
typedef struct _P2P_ATTRIBUTE_T {
	UINT_8	ucId;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	aucBody[1];	/*            */
} __KAL_ATTRIB_PACKED__ P2P_ATTRIBUTE_T, ATTRIBUTE_HDR_T, *P_P2P_ATTRIBUTE_T, *P_ATTRIBUTE_HDR_T;

/*                                          */
typedef struct _P2P_ATTRI_NOA_T {
    UINT_8      ucId;                   /*              */
    UINT_16     u2Length;               /*        */
    UINT_8      ucIndex;                /*       */
    UINT_8      ucCTWOppPSParam;        /*                               */
    UINT_8      aucNoADesc[1];          /*                */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_NOA_T, *P_P2P_ATTRI_NOA_T;

typedef struct _NOA_DESCRIPTOR_T {
    UINT_8      ucCountType;            /*            */
    UINT_32     u4Duration;             /*          */
    UINT_32     u4Interval;             /*          */
    UINT_32     u4StartTime;            /*            */
} __KAL_ATTRIB_PACKED__ NOA_DESCRIPTOR_T, *P_NOA_DESCRIPTOR_T;
typedef struct _CHANNEL_ENTRY_FIELD_T {
	UINT_8	ucRegulatoryClass;	/*                  */
	UINT_8	ucNumberOfChannels;	/*                    */
	UINT_8	aucChannelList[1];	/*              */
} __KAL_ATTRIB_PACKED__ CHANNEL_ENTRY_FIELD_T, *P_CHANNEL_ENTRY_FIELD_T;

#endif
