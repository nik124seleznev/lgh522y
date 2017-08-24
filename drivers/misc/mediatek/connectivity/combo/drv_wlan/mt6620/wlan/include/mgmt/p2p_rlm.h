/*
                                                                             
*/

/*                 
          
*/




#ifndef _P2P_RLM_H
#define _P2P_RLM_H


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


VOID rlmBssInitForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

BOOLEAN rlmUpdateBwByChListForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

VOID rlmUpdateParamsForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, BOOLEAN fgUpdateBeacon);

VOID rlmFuncInitialChannelList(IN P_ADAPTER_T prAdapter);

VOID
rlmFuncCommonChannelList(IN P_ADAPTER_T prAdapter,
			 IN P_CHANNEL_ENTRY_FIELD_T prChannelEntryII, IN UINT_8 ucChannelListSize);

UINT_8 rlmFuncFindOperatingClass(IN P_ADAPTER_T prAdapter, IN UINT_8 ucChannelNum);

BOOLEAN
rlmFuncFindAvailableChannel(IN P_ADAPTER_T prAdapter,
			    IN UINT_8 ucCheckChnl,
			    IN PUINT_8 pucSuggestChannel,
			    IN BOOLEAN fgIsSocialChannel, IN BOOLEAN fgIsDefaultChannel);

ENUM_CHNL_EXT_T rlmDecideScoForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

#endif
