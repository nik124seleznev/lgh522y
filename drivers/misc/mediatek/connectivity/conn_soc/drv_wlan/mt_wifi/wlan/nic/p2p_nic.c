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
VOID
nicRxAddP2pDevice (
    IN P_ADAPTER_T  prAdapter,
    IN P_EVENT_P2P_DEV_DISCOVER_RESULT_T prP2pResult,
    IN PUINT_8 pucRxIEBuf,
    IN UINT_16 u2RxIELength
    )
{
    P_P2P_INFO_T prP2pInfo = (P_P2P_INFO_T)NULL;
    P_EVENT_P2P_DEV_DISCOVER_RESULT_T prTargetResult = (P_EVENT_P2P_DEV_DISCOVER_RESULT_T)NULL;
    UINT_32 u4Idx = 0;
    BOOLEAN bUpdate = FALSE;

    PUINT_8 pucIeBuf = (PUINT_8)NULL;
    UINT_16 u2IELength = 0;
    UINT_8  zeroMac[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

    ASSERT(prAdapter);

    prP2pInfo = prAdapter->prP2pInfo;

    for (u4Idx = 0; u4Idx < prP2pInfo->u4DeviceNum; u4Idx++) {
        prTargetResult = &prP2pInfo->arP2pDiscoverResult[u4Idx];

        if (EQUAL_MAC_ADDR(prTargetResult->aucDeviceAddr, prP2pResult->aucDeviceAddr)) {
            bUpdate = TRUE;

            /*                           */
            pucIeBuf = prTargetResult->pucIeBuf;
            u2IELength = prTargetResult->u2IELength;

            /*                     */
            //     
            kalMemZero(prTargetResult, sizeof(EVENT_P2P_DEV_DISCOVER_RESULT_T));

            //            
            kalMemCopy(prTargetResult,
                (PVOID)prP2pResult,
                sizeof(EVENT_P2P_DEV_DISCOVER_RESULT_T));

            /*                                        */
            if ((u2RxIELength > u2IELength) && (u2IELength != 0)) {
                /*                       */
                u2RxIELength = u2IELength;
            }
            else if ((u2IELength == 0) && (u2RxIELength != 0)) {
                /*                */
                ASSERT(pucIeBuf == NULL);
                pucIeBuf = prP2pInfo->pucCurrIePtr;

                if (((ULONG)prP2pInfo->pucCurrIePtr + (ULONG)u2RxIELength) >
                        (ULONG)&prP2pInfo->aucCommIePool[CFG_MAX_COMMON_IE_BUF_LEN]) {
                    /*                             */
                    u2RxIELength = (UINT_16)((ULONG)&prP2pInfo->aucCommIePool[CFG_MAX_COMMON_IE_BUF_LEN] - (ULONG)prP2pInfo->pucCurrIePtr);
                }

                /*                              */
                prP2pInfo->pucCurrIePtr = (PUINT_8)((ULONG)prP2pInfo->pucCurrIePtr + (ULONG)u2RxIELength);
            }

            /*                         */
            prTargetResult->pucIeBuf = pucIeBuf;

            if (pucRxIEBuf) {
                /*                                
                                                               
                  */
                kalMemCopy(pucIeBuf, pucRxIEBuf, u2RxIELength);
                prTargetResult->u2IELength = u2RxIELength;
            }
            else {
                /*                                                   */
                prTargetResult->u2IELength = u2IELength;
            }
        }
    }

    if (!bUpdate) {
        /*                                                                        
                                                                 
          */
        if ((u4Idx < CFG_MAX_NUM_BSS_LIST) && (UNEQUAL_MAC_ADDR(zeroMac, prP2pResult->aucDeviceAddr))) { /*          */
            prTargetResult = &prP2pInfo->arP2pDiscoverResult[u4Idx];

            //     
            kalMemZero(prTargetResult, sizeof(EVENT_P2P_DEV_DISCOVER_RESULT_T));

            //            
            kalMemCopy(prTargetResult,
                (PVOID)prP2pResult,
                sizeof(EVENT_P2P_DEV_DISCOVER_RESULT_T));

            //                                                                                                                                                  

            if (u2RxIELength) {
                prTargetResult->pucIeBuf = prP2pInfo->pucCurrIePtr;

                if (((ULONG)prP2pInfo->pucCurrIePtr + (ULONG)u2RxIELength) >
                        (ULONG)&prP2pInfo->aucCommIePool[CFG_MAX_COMMON_IE_BUF_LEN]) {
                    /*                             */
                    u2IELength = (UINT_16)((ULONG)&prP2pInfo->aucCommIePool[CFG_MAX_COMMON_IE_BUF_LEN] - (ULONG)prP2pInfo->pucCurrIePtr);
                }
                else {
                    u2IELength = u2RxIELength;
                }

                prP2pInfo->pucCurrIePtr = (PUINT_8)((ULONG)prP2pInfo->pucCurrIePtr + (ULONG)u2IELength);

                kalMemCopy((PVOID)prTargetResult->pucIeBuf, (PVOID)pucRxIEBuf, (UINT_32)u2IELength);
                prTargetResult->u2IELength = u2IELength;
            }
            else {
                prTargetResult->pucIeBuf = NULL;
                prTargetResult->u2IELength = 0;
            }

            prP2pInfo->u4DeviceNum++;

        }
        else {
            //                                       
            ASSERT(FALSE);
        }
    }
} /*                   */


