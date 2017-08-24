/*
                                                                              
*/

/*                  
          
  
 */




#ifndef _P2P_SCAN_H
#define _P2P_SCAN_H

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


VOID scnEventReturnChannel(IN P_ADAPTER_T prAdapter, IN UINT_8 ucScnSeqNum);

BOOLEAN scanUpdateP2pDeviceDesc(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc);

VOID
scanP2pProcessBeaconAndProbeResp(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb, IN P_WLAN_STATUS prStatus, IN
				 P_BSS_DESC_T prBssDesc, IN P_WLAN_BEACON_FRAME_T prWlanBeaconFrame);

VOID scanRemoveAllP2pBssDesc(P_ADAPTER_T prAdapter);

VOID scanRemoveP2pBssDesc(P_ADAPTER_T prAdapter, P_BSS_DESC_T prBssDesc);


P_BSS_DESC_T
scanP2pSearchDesc(IN P_ADAPTER_T prAdapter, IN P_P2P_CONNECTION_REQ_INFO_T prConnReqInfo);



#endif
