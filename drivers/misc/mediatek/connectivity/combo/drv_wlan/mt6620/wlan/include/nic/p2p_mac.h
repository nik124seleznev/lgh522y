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

#define P2P_PUBLIC_ACTION_FRAME_LEN                (WLAN_MAC_MGMT_HEADER_LEN+8)
#define P2P_ACTION_FRAME_LEN                       (WLAN_MAC_MGMT_HEADER_LEN+7)


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


/*                                            */
typedef struct _P2P_PUBLIC_ACTION_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                              */
	UINT_8 ucCategory;	/*                */
	UINT_8 ucAction;	/*                    */
	UINT_8 aucOui[3];	/*                  */
	UINT_8 ucOuiType;	/*      */
	UINT_8 ucOuiSubtype;	/*                                                                            */
	UINT_8 ucDialogToken;	/*               */
	UINT_8 aucInfoElem[1];	/*                 */
} __KAL_ATTRIB_PACKED__ P2P_PUBLIC_ACTION_FRAME_T, *P_P2P_PUBLIC_ACTION_FRAME_T;


/*                                             */
typedef struct _P2P_ACTION_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                       */
	UINT_8 ucCategory;	/*      */
	UINT_8 aucOui[3];	/*                  */
	UINT_8 ucOuiType;	/*      */
	UINT_8 ucOuiSubtype;	/*  */
	UINT_8 ucDialogToken;
	UINT_8 aucInfoElem[1];
} __KAL_ATTRIB_PACKED__ P2P_ACTION_FRAME_T, *P_P2P_ACTION_FRAME_T;

/*                                                        */
typedef struct _GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                              */
	UINT_8 ucCategory;	/*                */
	UINT_8 ucAction;	/*                    */
	UINT_8 ucDialogToken;	/*               */
	UINT_8 aucInfoElem[1];	/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T,
    *P_GAS_PUBLIC_ACTION_INITIAL_REQUEST_FRAME_T;

/*                                                         */
typedef struct _GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                              */
	UINT_8 ucCategory;	/*                */
	UINT_8 ucAction;	/*                    */
	UINT_8 ucDialogToken;	/*               */
	UINT_16 u2StatusCode;	/*                   */
	UINT_16 u2ComebackDelay;	/*                   *//*                */
	UINT_8 aucInfoElem[1];	/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T,
    *P_GAS_PUBLIC_ACTION_INITIAL_RESPONSE_FRAME_T;


/*                                                           */
typedef struct _GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                              */
	UINT_8 ucCategory;	/*                */
	UINT_8 ucAction;	/*                    */
	UINT_8 ucDialogToken;	/*               */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T,
    *P_GAS_PUBLIC_ACTION_COMEBACK_REQUEST_FRAME_T;

/*                                                            */
typedef struct _GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T {
	/*            */
	UINT_16 u2FrameCtrl;	/*               */
	UINT_16 u2Duration;	/*          */
	UINT_8 aucDestAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucSrcAddr[MAC_ADDR_LEN];	/*    */
	UINT_8 aucBSSID[MAC_ADDR_LEN];	/*       */
	UINT_16 u2SeqCtrl;	/*                  */
	/*                              */
	UINT_8 ucCategory;	/*                */
	UINT_8 ucAction;	/*                    */
	UINT_8 ucDialogToken;	/*               */
	UINT_16 u2StatusCode;	/*                    */
	UINT_8 ucFragmentID;	/*                   */
	UINT_16 u2ComebackDelay;	/*                    */
	UINT_8 aucInfoElem[1];	/*                   */
} __KAL_ATTRIB_PACKED__ GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T,
    *P_GAS_PUBLIC_ACTION_COMEBACK_RESPONSE_FRAME_T;




typedef struct _P2P_SD_VENDER_SPECIFIC_CONTENT_T {
	/*                                            */
	UINT_8 ucOuiSubtype;	/*      */
	UINT_16 u2ServiceUpdateIndicator;
	UINT_8 aucServiceTLV[1];
} __KAL_ATTRIB_PACKED__ P2P_SD_VENDER_SPECIFIC_CONTENT_T, *P_P2P_SD_VENDER_SPECIFIC_CONTENT_T;


typedef struct _P2P_SERVICE_REQUEST_TLV_T {
	UINT_16 u2Length;
	UINT_8 ucServiceProtocolType;
	UINT_8 ucServiceTransID;
	UINT_8 aucQueryData[1];
} __KAL_ATTRIB_PACKED__ P2P_SERVICE_REQUEST_TLV_T, *P_P2P_SERVICE_REQUEST_TLV_T;



typedef struct _P2P_SERVICE_RESPONSE_TLV_T {
	UINT_16 u2Length;
	UINT_8 ucServiceProtocolType;
	UINT_8 ucServiceTransID;
	UINT_8 ucStatusCode;
	UINT_8 aucResponseData[1];
} __KAL_ATTRIB_PACKED__ P2P_SERVICE_RESPONSE_TLV_T, *P_P2P_SERVICE_RESPONSE_TLV_T;


#endif
