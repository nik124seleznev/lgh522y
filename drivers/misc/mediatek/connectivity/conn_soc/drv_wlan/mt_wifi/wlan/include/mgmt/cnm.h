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

    CH_REQ_TYPE_NUM
} ENUM_CH_REQ_TYPE_T, *P_ENUM_CH_REQ_TYPE_T;

typedef struct _MSG_CH_REQ_T {
    MSG_HDR_T           rMsgHdr;    /*                          */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
    UINT_32             u4MaxInterval;  /*               */
    UINT_8              aucBSSID[6];
    UINT_8              aucReserved[2];
} MSG_CH_REQ_T, *P_MSG_CH_REQ_T;

typedef struct _MSG_CH_ABORT_T {
    MSG_HDR_T           rMsgHdr;    /*                          */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
} MSG_CH_ABORT_T, *P_MSG_CH_ABORT_T;

typedef struct _MSG_CH_GRANT_T {
    MSG_HDR_T           rMsgHdr;    /*                          */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
    UINT_32             u4GrantInterval;    /*               */
} MSG_CH_GRANT_T, *P_MSG_CH_GRANT_T;

typedef struct _MSG_CH_REOCVER_T {
    MSG_HDR_T           rMsgHdr;    /*                          */
    UINT_8              ucNetTypeIndex;
    UINT_8              ucTokenID;
    UINT_8              ucPrimaryChannel;
    ENUM_CHNL_EXT_T     eRfSco;
    ENUM_BAND_T         eRfBand;
    ENUM_CH_REQ_TYPE_T  eReqType;
} MSG_CH_RECOVER_T, *P_MSG_CH_RECOVER_T;


typedef struct _CNM_INFO_T {
    UINT_32     u4Reserved;
} CNM_INFO_T, *P_CNM_INFO_T;

#if CFG_ENABLE_WIFI_DIRECT
/*                      */
typedef struct _DEVICE_TYPE_T {
    UINT_16     u2CategoryId;           /*             */
    UINT_8      aucOui[4];              /*     */
    UINT_16     u2SubCategoryId;        /*                 */
} __KAL_ATTRIB_PACKED__ DEVICE_TYPE_T, *P_DEVICE_TYPE_T;
#endif


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/
VOID
cnmInit (
    P_ADAPTER_T prAdapter
    );

VOID
cnmUninit (
    P_ADAPTER_T     prAdapter
    );

VOID
cnmChMngrRequestPrivilege (
    P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
    );

VOID
cnmChMngrAbortPrivilege (
    P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
    );

VOID
cnmChMngrHandleChEvent (
    P_ADAPTER_T     prAdapter,
    P_WIFI_EVENT_T  prEvent
    );

BOOLEAN
cnmPreferredChannel (
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel,
    P_ENUM_CHNL_EXT_T   prBssSCO
    );

BOOLEAN
cnmAisInfraChannelFixed (
    P_ADAPTER_T         prAdapter,
    P_ENUM_BAND_T       prBand,
    PUINT_8             pucPrimaryChannel
    );

VOID
cnmAisInfraConnectNotify (
    P_ADAPTER_T         prAdapter
    );

BOOLEAN
cnmAisIbssIsPermitted (
    P_ADAPTER_T     prAdapter
    );

BOOLEAN
cnmP2PIsPermitted (
    P_ADAPTER_T     prAdapter
    );

BOOLEAN
cnmBowIsPermitted (
    P_ADAPTER_T     prAdapter
    );

BOOLEAN
cnmBss40mBwPermitted (
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIdx
    );

/*                                                                              
                                                
                                                                                
*/
#ifndef _lint
/*                                                                        
                                                     
                                                                             
                                               
 */
__KAL_INLINE__ VOID
cnmMsgDataTypeCheck (
    VOID
    )
{
    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,rMsgHdr) == 0);

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,rMsgHdr) ==
        OFFSET_OF(MSG_CH_RECOVER_T,rMsgHdr));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,ucNetTypeIndex) ==
        OFFSET_OF(MSG_CH_RECOVER_T,ucNetTypeIndex));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,ucTokenID) ==
        OFFSET_OF(MSG_CH_RECOVER_T,ucTokenID));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,ucPrimaryChannel) ==
        OFFSET_OF(MSG_CH_RECOVER_T,ucPrimaryChannel));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,eRfSco) ==
        OFFSET_OF(MSG_CH_RECOVER_T,eRfSco));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,eRfBand) ==
        OFFSET_OF(MSG_CH_RECOVER_T,eRfBand));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSG_CH_GRANT_T,eReqType) ==
        OFFSET_OF(MSG_CH_RECOVER_T,eReqType));

    return;
}
#endif /*       */

#endif /*        */


