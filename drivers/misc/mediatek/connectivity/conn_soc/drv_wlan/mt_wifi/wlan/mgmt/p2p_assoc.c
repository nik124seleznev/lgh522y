/*
                          
*/

/*                       
                                                                              

                                                         
*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#include "precomp.h"

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

/*                                                                            */
/* 
                                                                                         
                       
 
                                                                
 
                
*/
/*                                                                            */
PUINT_8
p2pBuildReAssocReqFrameCommonIEs(
    IN P_ADAPTER_T prAdapter,
    IN P_MSDU_INFO_T prMsduInfo,
    IN PUINT_8 pucBuffer
    )
{
    P_P2P_CONNECTION_SETTINGS_T prP2pConnSettings = (P_P2P_CONNECTION_SETTINGS_T)NULL;

    prP2pConnSettings = prAdapter->rWifiVar.prP2PConnSettings;

    /*                        */
    SSID_IE(pucBuffer)->ucId = ELEM_ID_SSID;

    /*                                                                       
                                                                                 
     */

    COPY_SSID(SSID_IE(pucBuffer)->aucSSID,
              SSID_IE(pucBuffer)->ucLength,
              prP2pConnSettings->aucSSID,
              prP2pConnSettings->ucSSIDLen);

    prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
    pucBuffer += IE_SIZE(pucBuffer);
    return pucBuffer;
}


