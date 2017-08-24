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
VOID cmdBufInitialize(IN P_ADAPTER_T prAdapter)
{
	P_CMD_INFO_T prCmdInfo;
	UINT_32 i;

	ASSERT(prAdapter);

	QUEUE_INITIALIZE(&prAdapter->rFreeCmdList);

	for (i = 0; i < CFG_TX_MAX_CMD_PKT_NUM; i++) {
		prCmdInfo = &prAdapter->arHifCmdDesc[i];
		QUEUE_INSERT_TAIL(&prAdapter->rFreeCmdList, &prCmdInfo->rQueEntry);
	}

}				/*                           */


/*                                                                            */
/* 
                                                                   
 
                                                             
                                                                   
 
                                                           
                                              
*/
/*                                                                            */
P_CMD_INFO_T cmdBufAllocateCmdInfo(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Length)
{
	P_CMD_INFO_T prCmdInfo;
	KAL_SPIN_LOCK_DECLARATION();

	DEBUGFUNC("cmdBufAllocateCmdInfo");


	ASSERT(prAdapter);

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
	QUEUE_REMOVE_HEAD(&prAdapter->rFreeCmdList, prCmdInfo, P_CMD_INFO_T);
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);

	if (prCmdInfo) {
		/*                                   */
		/*                                   */
		prCmdInfo->pucInfoBuffer = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, u4Length);

		if (prCmdInfo->pucInfoBuffer == NULL) {
			KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
			QUEUE_INSERT_TAIL(&prAdapter->rFreeCmdList, &prCmdInfo->rQueEntry);
			KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);

			prCmdInfo = NULL;
		} else {
			prCmdInfo->u2InfoBufLen = 0;
			prCmdInfo->fgIsOid = FALSE;
			prCmdInfo->fgDriverDomainMCR = FALSE;
		}
	}

	if (prCmdInfo) {
		DBGLOG(MEM, INFO,
		       ("CMD[0x%p] allocated! LEN[%04u], Rest[%u]\n", prCmdInfo, u4Length,
			prAdapter->rFreeCmdList.u4NumElem));
	} else {
		DBGLOG(MEM, INFO,
		       ("CMD allocation failed! LEN[%04u], Rest[%u]\n", u4Length,
			prAdapter->rFreeCmdList.u4NumElem));
	}

	return prCmdInfo;

}				/*                                */


/*                                                                            */
/* 
                                                                              
 
                                                     
                                      
 
                
*/
/*                                                                            */
VOID cmdBufFreeCmdInfo(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo)
{
	KAL_SPIN_LOCK_DECLARATION();

	DEBUGFUNC("cmdBufFreeCmdInfo");

	ASSERT(prAdapter);

	if (prCmdInfo) {
		if (prCmdInfo->pucInfoBuffer) {
			cnmMemFree(prAdapter, prCmdInfo->pucInfoBuffer);
			prCmdInfo->pucInfoBuffer = NULL;
		}

		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
		QUEUE_INSERT_TAIL(&prAdapter->rFreeCmdList, &prCmdInfo->rQueEntry);
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
	}

	if (prCmdInfo) {
		DBGLOG(MEM, INFO,
		       ("CMD[0x%p] freed! Rest[%u]\n", prCmdInfo,
			prAdapter->rFreeCmdList.u4NumElem));
	}

	return;

}				/*                              */
