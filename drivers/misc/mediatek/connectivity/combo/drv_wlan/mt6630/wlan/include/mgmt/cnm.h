/*
                                                                                    
*/

/*                 
          
*/



/*
                
  
                        
                                                                                         
                       
  
                      
                                                             
                                                      
  
                        
                                                                                         
                     
  
                      
                                                             
                                  
                                                                
  
                      
                                                             
                                              
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                                                  
                                               
  
                   
                                                                                                                  
                                                               
                                               
  
                      
                                                                                                      
    
  
                      
                                                                                        
                                                                         
  
                      
                                                                                        
                                                            
  
                      
                                                                                      
                                   
                                                       
  
                      
       
                                                     
  
                      
  
                                                    
  
                      
  
                                            
  
                   
  
                                                                               
  
                      
                                                            
                                                        
  
                      
                                                            
                                                        
  
                      
                                                            
                                          
  
                         
                                                               
                                                 
  
                      
                                                               
                                           
  
                         
                                                          
                 
  
                         
                                                                       
                                                           
  
                      
                                                               
                                                     
  
                       
                                                                
                                     
  
                      
                                                                
  
  
*/

#ifndef _CNM_H
#define _CNM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

typedef enum _ENUM_CH_REQ_TYPE_T {
	CH_REQ_TYPE_JOIN,
	CH_REQ_TYPE_P2P_LISTEN,
	CH_REQ_TYPE_OFFCHNL_TX,
	CH_REQ_TYPE_GO_START_BSS,
	CH_REQ_TYPE_NUM
} ENUM_CH_REQ_TYPE_T, *P_ENUM_CH_REQ_TYPE_T;

typedef struct _MSG_CH_REQ_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/*                                */
	UINT_8 ucRfCenterFreqSeg1;	/*                                */
	UINT_8 ucRfCenterFreqSeg2;	/*                                */
	ENUM_CH_REQ_TYPE_T eReqType;
	UINT_32 u4MaxInterval;	/*               */
} MSG_CH_REQ_T, *P_MSG_CH_REQ_T;

typedef struct _MSG_CH_ABORT_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
} MSG_CH_ABORT_T, *P_MSG_CH_ABORT_T;

typedef struct _MSG_CH_GRANT_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/*                                */
	UINT_8 ucRfCenterFreqSeg1;	/*                                */
	UINT_8 ucRfCenterFreqSeg2;	/*                                */
	ENUM_CH_REQ_TYPE_T eReqType;
	UINT_32 u4GrantInterval;	/*               */
} MSG_CH_GRANT_T, *P_MSG_CH_GRANT_T;

typedef struct _MSG_CH_REOCVER_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
	UINT_8 ucPrimaryChannel;
	ENUM_CHNL_EXT_T eRfSco;
	ENUM_BAND_T eRfBand;
	ENUM_CHANNEL_WIDTH_T eRfChannelWidth;	/*                                */
	UINT_8 ucRfCenterFreqSeg1;	/*                                */
	UINT_8 ucRfCenterFreqSeg2;	/*                                */
	ENUM_CH_REQ_TYPE_T eReqType;
} MSG_CH_RECOVER_T, *P_MSG_CH_RECOVER_T;


typedef struct _CNM_INFO_T {
	BOOLEAN fgChGranted;
	UINT_8 ucBssIndex;
	UINT_8 ucTokenID;
} CNM_INFO_T, *P_CNM_INFO_T;

#if CFG_ENABLE_WIFI_DIRECT
/*                      */
typedef struct _DEVICE_TYPE_T {
	UINT_16 u2CategoryId;	/*             */
	UINT_8 aucOui[4];	/*     */
	UINT_16 u2SubCategoryId;	/*                 */
} __KAL_ATTRIB_PACKED__ DEVICE_TYPE_T, *P_DEVICE_TYPE_T;
#endif


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define CNM_CH_GRANTED_FOR_BSS(_prAdapter, _ucBssIndex) \
	((_prAdapter)->rCnmInfo.fgChGranted && \
	 (_prAdapter)->rCnmInfo.ucBssIndex == (_ucBssIndex))

/*                                                                              
                                                             
                                                                                
*/
VOID cnmInit(P_ADAPTER_T prAdapter);

VOID cnmUninit(P_ADAPTER_T prAdapter);

VOID cnmChMngrRequestPrivilege(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID cnmChMngrAbortPrivilege(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID cnmChMngrHandleChEvent(P_ADAPTER_T prAdapter, P_WIFI_EVENT_T prEvent);

BOOLEAN
cnmPreferredChannel(P_ADAPTER_T prAdapter,
		    P_ENUM_BAND_T prBand, PUINT_8 pucPrimaryChannel, P_ENUM_CHNL_EXT_T prBssSCO);

BOOLEAN
cnmAisInfraChannelFixed(P_ADAPTER_T prAdapter, P_ENUM_BAND_T prBand, PUINT_8 pucPrimaryChannel);

VOID cnmAisInfraConnectNotify(P_ADAPTER_T prAdapter);

BOOLEAN cnmAisIbssIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmP2PIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmBowIsPermitted(P_ADAPTER_T prAdapter);

BOOLEAN cnmBss40mBwPermitted(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

BOOLEAN cnmBss80mBwPermitted(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

UINT_8 cnmGetBssMaxBw(P_ADAPTER_T prAdapter, UINT_8 ucBssIndex);

P_BSS_INFO_T
cnmGetBssInfoAndInit(P_ADAPTER_T prAdapter,
		     ENUM_NETWORK_TYPE_T eNetworkType, BOOLEAN fgIsP2pDevice);

VOID cnmFreeBssInfo(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

/*                                                                              
                                                
                                                                                
*/
#ifndef _lint
/*                                                                        
                                                     
                                                                             
                                               
 */
static __KAL_INLINE__ VOID cnmMsgDataTypeCheck(VOID)
{
	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) == 0);

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, rMsgHdr) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, rMsgHdr));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucBssIndex) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, ucBssIndex));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucTokenID) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, ucTokenID));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, ucPrimaryChannel) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, ucPrimaryChannel));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eRfSco) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, eRfSco));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eRfBand) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, eRfBand));

	DATA_STRUC_INSPECTING_ASSERT(OFFSET_OF(MSG_CH_GRANT_T, eReqType) ==
				     OFFSET_OF(MSG_CH_RECOVER_T, eReqType));

	return;
}
#endif				/*       */

#endif				/*        */
