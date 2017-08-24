/*
                                                                                  
*/

/*                      
          
*/



#ifndef _P2P_RLM_OBSS_H
#define _P2P_RLM_OBSS_H

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

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

VOID
rlmRspGenerateObssScanIE (
    P_ADAPTER_T     prAdapter,
    P_MSDU_INFO_T   prMsduInfo
    );

VOID
rlmProcessPublicAction (
    P_ADAPTER_T prAdapter,
    P_SW_RFB_T  prSwRfb
    );

VOID
rlmProcessHtAction (
    P_ADAPTER_T prAdapter,
    P_SW_RFB_T  prSwRfb
    );

VOID
rlmHandleObssStatusEventPkt (
    P_ADAPTER_T                 prAdapter,
    P_EVENT_AP_OBSS_STATUS_T    prObssStatus
    );

UINT_8
rlmObssChnlLevel (
    P_BSS_INFO_T        prBssInfo,
    ENUM_BAND_T         eBand,
    UINT_8              ucPriChannel,
    ENUM_CHNL_EXT_T     eExtend
    );

VOID
rlmObssScanExemptionRsp (
    P_ADAPTER_T         prAdapter,
    P_BSS_INFO_T        prBssInfo,
    P_SW_RFB_T          prSwRfb
    );

#endif
